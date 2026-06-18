/*
 * Exploration.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Rôle :
 *   - détecter les voisins via MCP23017
 *   - créer les aiguilles logiques (Aig)
 *   - construire la topologie locale SP1 / SM1 / SP2 / SM2
 *   - envoyer la topologie vers l’Extension Canton Controller (EXCC)
 *
 * IMPORTANT 2026 :
 *   - aucun signal n’est imposé ici
 *   - aucun masque d’aiguilles n’est géré ici
 *   - la logique métier (aspects, mâts, sécurité) est ailleurs
 *   - Exploration ne fait QUE la topologie locale
 */

#include "Exploration.h"
#include "debug_cc.h"

// ---------------------------------------------------------------------------
// Variables statiques
// ---------------------------------------------------------------------------
byte Exploration::m_btnState{0};
byte Exploration::m_ID_satPeriph{UNUSED_ID};
byte Exploration::m_comptAig{0};
bool Exploration::m_stopProcess{false};
Canton *Exploration::canton = nullptr;

// ---------------------------------------------------------------------------
// Accesseurs
// ---------------------------------------------------------------------------
void Exploration::comptAig(byte v) { m_comptAig = v; }
byte Exploration::comptAig() { return m_comptAig; }

void Exploration::ID_satPeriph(byte v) { m_ID_satPeriph = v; }
byte Exploration::ID_satPeriph() { return m_ID_satPeriph; }

void Exploration::btnState(byte v) { m_btnState = v; }
byte Exploration::btnState() { return m_btnState; }

void Exploration::stopProcess(bool v) { m_stopProcess = v; }

// ---------------------------------------------------------------------------
// Fonction interne : une PASSE de découverte
// ---------------------------------------------------------------------------
static void runExplorationPass(Canton *canton)
{
    // Remise à zéro du compteur d’aiguilles
    Exploration::comptAig(0);

    // --------------------------------------------------------
    // Création logique des aiguilles
    // --------------------------------------------------------
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

        // On ne gère plus de masque d’aiguilles ici (2026)
        Exploration::comptAig(Exploration::comptAig() + 1);
    };

    // Conditions de création des aiguilles (héritées de Discovery)
    const byte aigConditions[aigSize][2] = {
        {p00, p01}, {p00, p10}, {p01, p11},
        {m00, m01}, {m00, m10}, {m01, m11}
    };

    for (uint8_t i = 0; i < aigSize; i++)
    {
        auto c = aigConditions[i];
        if (canton->getCantonP(c[0]) && canton->getCantonP(c[1]))
            createAig(i, c[0], c[1]);
    }

    // --------------------------------------------------------
    // Envoi de la topologie vers EXCC
    // --------------------------------------------------------
    envoyerTopologieSiPret();
}

// ---------------------------------------------------------------------------
// begin() — initialisation du mode Exploration
// ---------------------------------------------------------------------------
void Exploration::begin(Canton *nd)
{
    canton = nd;

    // Boutons via MCP23017
    canton->mcp.pinMode(MCP_PIN_BTN_SAT_MOINS, INPUT_PULLUP);
    canton->mcp.pinMode(MCP_PIN_BTN_SAT_PLUS,  INPUT_PULLUP);
    canton->mcp.pinMode(MCP_PIN_INTER_DEV_2,   INPUT_PULLUP);
    canton->mcp.pinMode(MCP_PIN_INTER_DEV_1,   INPUT_PULLUP);

    // LED Exploration
    canton->mcp.pinMode(MCP_PIN_LED_EXPLORATION, OUTPUT);

    // Tâches FreeRTOS
    xTaskCreatePinnedToCore(process,
                            "Process",
                            8192,
                            (void *)canton,
                            7,
                            NULL,
                            1);

    xTaskCreatePinnedToCore(createAigEtCibles,
                            "CreateAiguilles",
                            8192,
                            (void *)canton,
                            2,
                            NULL,
                            0);
}

// ---------------------------------------------------------------------------
// process() — gestion boutons + reset logique + CAN
// ---------------------------------------------------------------------------
void Exploration::process(void *p)
{
    Canton *canton = (Canton *)p;
    bool ledAllumee = false;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    auto clignoterLED = [&]()
    {
        canton->mcp.digitalWrite(MCP_PIN_LED_EXPLORATION,
                                 ledAllumee ? HIGH : LOW);
        ledAllumee = !ledAllumee;
    };

    auto allumerLED = [&]()
    {
        canton->mcp.digitalWrite(MCP_PIN_LED_EXPLORATION, HIGH);
    };

    auto eteindreLED = [&]()
    {
        canton->mcp.digitalWrite(MCP_PIN_LED_EXPLORATION, LOW);
    };

    auto btnPush = [&](uint8_t btnNum)
    {
        // Notification CAN vers la carte Main
        CanMsg::sendMsg(0, 0xC0, 0, canton->ID(), UNUSED_ID, 0);

        if (m_ID_satPeriph < 253)
        {
            CantonPeriph *np = canton->getCantonP(btnNum);
            if (!np)
            {
                np = new CantonPeriph;
                canton->setCantonP(btnNum, np);
            }

            np->ID(15); // TODO : attribution dynamique plus tard
            allumerLED();
            m_ID_satPeriph = UNUSED_ID;
        }
        else
        {
            clignoterLED();
        }
    };

    for (;;)
    {
        // Lecture boutons via MCP23017
        bool satMoins = !canton->mcp.digitalRead(MCP_PIN_BTN_SAT_MOINS);
        bool satPlus  = !canton->mcp.digitalRead(MCP_PIN_BTN_SAT_PLUS);
        bool dev2     = !canton->mcp.digitalRead(MCP_PIN_INTER_DEV_2);
        bool dev1     = !canton->mcp.digitalRead(MCP_PIN_INTER_DEV_1);

        m_btnState =
            (satMoins ? 0x01 : 0) |
            (satPlus  ? 0x02 : 0) |
            (dev2     ? 0x04 : 0) |
            (dev1     ? 0x08 : 0);

        switch (m_btnState & 0x03)
        {
        case 0x01:
            btnPush(m_btnState >> 2);
            break;

        case 0x02:
            btnPush((m_btnState >> 2) + 4);
            break;

        case 0x03: // RESET LOGIQUE + nouvelle découverte
            // Réinitialisation des cantons périphériques
            for (byte i = 0; i < cantonPsize; i++)
            {
                CantonPeriph *np = canton->getCantonP(i);
                if (np)
                {
                    np->ID(UNUSED_ID);
                    np->busy(false);
                    np->reserved(0);
                    np->masqueAig(0); // masque côté CantonPeriph : OK
                }
            }

            // Réinitialisation des aiguilles
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

            // Réinitialisation des signaux (aucun type imposé)
            for (byte i = 0; i < signalSize; i++)
            {
                Signal *s = canton->getSignal(i);
                if (s)
                {
                    s->type(0);
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

// ---------------------------------------------------------------------------
// createAigEtCibles() — 1ère passe au boot
// ---------------------------------------------------------------------------
void Exploration::createAigEtCibles(void *p)
{
    Canton *canton = (Canton *)p;

    runExplorationPass(canton);

    vTaskDelete(NULL);
}
