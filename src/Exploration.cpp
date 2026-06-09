/*
  Exploration.cpp — Version 2026 (STABLE & SAFE)
  ------------------------------------------------------------
*/

#include "Exploration.h"

// ------------------------------------------------------------
// Variables statiques
// ------------------------------------------------------------
byte Exploration::m_btnState{0};
byte Exploration::m_ID_satPeriph{UNUSED_ID};
byte Exploration::m_comptAig{0};
bool Exploration::m_stopProcess{false};
Canton *Exploration::canton = nullptr;

const gpio_num_t Exploration::m_pinIn[] = {
    BTN_SAT_MOINS, BTN_SAT_PLUS, INTER_DEV_2, INTER_DEV_1};

const gpio_num_t Exploration::m_pinLed = LED_PIN_DISCOV;

// ------------------------------------------------------------
// Accesseurs
// ------------------------------------------------------------
void Exploration::comptAig(byte v) { m_comptAig = v; }
byte Exploration::comptAig() { return m_comptAig; }

void Exploration::ID_satPeriph(byte v) { m_ID_satPeriph = v; }
byte Exploration::ID_satPeriph() { return m_ID_satPeriph; }

void Exploration::btnState(byte v) { m_btnState = v; }
byte Exploration::btnState() { return m_btnState; }

void Exploration::stopProcess(bool v) { m_stopProcess = v; }

// ------------------------------------------------------------
// Fonction interne : une PASSE de découverte
// ------------------------------------------------------------
static void runExplorationPass(Canton *canton)
{
    canton->masqueAig(0x00);
    Exploration::comptAig(0);

    // ------------------------------
    // Création logique des aiguilles
    // ------------------------------
    auto createAig = [&](uint8_t index, uint8_t nodP0, uint8_t nodP1)
    {
        Aig *a = canton->getAig(index);
        if (!a)
        {
            a = new Aig;
            canton->setAig(index, a);
        }

        a->ID(index);
        a->cantonPdroitIdx(nodP0);
        a->cantonPdevieIdx(nodP1);

        canton->masqueAig(canton->masqueAig() | (1 << index));
        Exploration::comptAig(Exploration::comptAig() + 1);
    };

    const byte aigConditions[aigSize][2] = {
        {p00, p01}, {p00, p10}, {p01, p11}, {m00, m01}, {m00, m10}, {m01, m11}};

    for (uint8_t i = 0; i < aigSize; i++)
    {
        auto c = aigConditions[i];
        if (canton->getCantonP(c[0]) && canton->getCantonP(c[1]))
            createAig(i, c[0], c[1]);
    }

    // ------------------------------
    // Signaux H / AH
    // ------------------------------
    for (byte sens = 0; sens < 2; sens++)
    {
        uint8_t x = (sens == 0 ? 0 : 3);
        uint8_t y = (sens == 0 ? 3 : 0);
        uint8_t index = (sens == 0 ? p00 : m00);
        uint8_t sigIdx = sens;

        byte typeCible = 0;

        if (canton->masqueAig() & (1 << x))
            typeCible = 3;
        else if (canton->getCantonP(index))
        {
            if (canton->getCantonP(index)->masqueAig() & (1 << y))
                typeCible = 1;
            else if (canton->getCantonP(index)->masqueAig() & (1 << x))
                typeCible = 2;
        }
        else
            typeCible = 1;

        Signal *s = canton->getSignal(sigIdx);
        if (s)
        {
            s->type(typeCible);
            s->position(sigIdx);
            s->setup();
        }
    }

    // Sauvegarde + envoi topologie
    // Settings::writeFile(Settings::canton);
    envoyerTopologieSiPret();
}

// ------------------------------------------------------------
// begin()
// ------------------------------------------------------------
void Exploration::begin(Canton *nd)
{
    canton = nd;

    for (byte i = 0; i < 4; i++)
        pinMode(m_pinIn[i], INPUT);

    pinMode(m_pinLed, OUTPUT);

    // Stack augmentés pour éviter les stack overflows
    xTaskCreatePinnedToCore(process, "Process", 8192, (void *)canton, 7, NULL, 1);
    xTaskCreatePinnedToCore(createAigEtCibles, "CreateAiguilles", 8192, (void *)canton, 2, NULL, 0);
}

// ------------------------------------------------------------
// process() — gestion boutons + reset logique + CAN
// ------------------------------------------------------------
void Exploration::process(void *p)
{
    Canton *canton = (Canton *)p;
    bool ledAllumee = LOW;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    auto clignoterLED = [&]()
    {
        digitalWrite(m_pinLed, ledAllumee ? LOW : HIGH);
        ledAllumee = !ledAllumee;
    };

    auto allumerLED = [&]()
    { digitalWrite(m_pinLed, HIGH); };
    auto eteindreLED = [&]()
    { digitalWrite(m_pinLed, LOW); };

    auto btnPush = [&](uint8_t btnNum)
    {
        CanMsg::sendMsg(0, 0xC0, 0, canton->ID(), UNUSED_ID, 0);

        if (m_ID_satPeriph < 253)
        {
            CantonPeriph *np = canton->getCantonP(btnNum);
            if (!np)
            {
                np = new CantonPeriph;
                canton->setCantonP(btnNum, np);
            }

            np->ID(15); // m_ID_satPeriph
            allumerLED();
            m_ID_satPeriph = UNUSED_ID;
        }
        else
            clignoterLED();
    };

    for (;;)
    {
        // Lecture boutons
        for (byte i = 0; i < 4; i++)
        {
            if (!digitalRead(m_pinIn[i]))
                m_btnState |= (1 << i);
            else
                m_btnState &= ~(1 << i);
        }

        switch (m_btnState & 0x03)
        {
        case 0x01:
            btnPush(m_btnState >> 2);
            break;

        case 0x02:
            btnPush((m_btnState >> 2) + 4);
            break;

        case 0x03: // RESET LOGIQUE + nouvelle découverte
            for (byte i = 0; i < cantonPsize; i++)
            {
                CantonPeriph *np = canton->getCantonP(i);
                if (np)
                {
                    np->ID(UNUSED_ID);
                    np->busy(false);
                    np->reserved(0);
                    np->masqueAig(0);
                }
            }

            for (byte i = 0; i < aigSize; i++)
            {
                Aig *a = canton->getAig(i);
                if (a)
                {
                    a->ID(i);
                    a->cantonPdroitIdx(0);
                    a->cantonPdevieIdx(0);
                }
            }

            for (byte i = 0; i < signalSize; i++)
            {
                Signal *s = canton->getSignal(i);
                if (s)
                {
                    s->type(SIG_SIMPLE);
                    s->position(i);
                    s->setup();
                }
            }

            m_comptAig = 0;
            allumerLED();

            runExplorationPass(canton);
            break;

        default:
            eteindreLED();
            break;
        }

        if (m_stopProcess)
            vTaskDelete(NULL);

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
    }
}

// ------------------------------------------------------------
// createAigEtCibles() — 1ère passe au boot
// ------------------------------------------------------------
void Exploration::createAigEtCibles(void *p)
{
    Canton *canton = (Canton *)p;

    runExplorationPass(canton);

    vTaskDelete(NULL);
}
