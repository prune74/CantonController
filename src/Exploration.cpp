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
 *
 * AJOUT 2026 :
 *   - bouton MANOEUVRE (voie de service)
 *   - LED MANOEUVRE dédiée
 *   - LED MANOEUVRE = reflète l’état du mode manœuvre **uniquement en exploration**
 *   - LED MANOEUVRE = toujours OFF en exploitation
 *   - LED Exploration = uniquement pour la découverte
 */

#include "Exploration.h"
#include "CC_CAN_EXCC.h"
#include "Settings.h"
#include "debug_cc.h"

// ---------------------------------------------------------------------------
// Variables statiques
// ---------------------------------------------------------------------------
uint8_t Exploration::m_btnState{0};
uint8_t Exploration::m_ID_satPeriph{UNUSED_ID};
uint8_t Exploration::m_comptAig{0};
bool Exploration::m_stopProcess{false};
Canton *Exploration::canton = nullptr;

// ---------------------------------------------------------------------------
// Accesseurs
// ---------------------------------------------------------------------------
void Exploration::comptAig(uint8_t v) { m_comptAig = v; } // 🟢
uint8_t Exploration::comptAig() { return m_comptAig; }    // 🟢

void Exploration::ID_satPeriph(uint8_t v) { m_ID_satPeriph = v; } // 🟢
uint8_t Exploration::ID_satPeriph() { return m_ID_satPeriph; }    // 🟢

void Exploration::btnState(uint8_t v) { m_btnState = v; } // 🟢
uint8_t Exploration::btnState() { return m_btnState; }    // 🟢

void Exploration::stopProcess(bool v) { m_stopProcess = v; } // 🟢

// ---------------------------------------------------------------------------
// Fonction interne : une PASSE de découverte
// ---------------------------------------------------------------------------
static void runExplorationPass(Canton *canton) // 🟢
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

        Exploration::comptAig(Exploration::comptAig() + 1);
    };

    // Conditions de création des aiguilles
    const uint8_t aigConditions[aigSize][2] = {
        {p00, p01}, {p00, p10}, {p01, p11}, {m00, m01}, {m00, m10}, {m01, m11}};

    for (uint8_t i = 0; i < aigSize; i++)
    {
        auto c = aigConditions[i];
        if (canton->getCantonP(c[0]) && canton->getCantonP(c[1]))
            createAig(i, c[0], c[1]);
    }
}

// ---------------------------------------------------------------------------
// begin() — initialisation du mode Exploration
// ---------------------------------------------------------------------------
void Exploration::begin(Canton *nd) // 🟢
{
    canton = nd;

    // Boutons via MCP23017
    canton->mcp.pinMode(MCP_PIN_BTN_CC_MOINS, INPUT_PULLUP);
    canton->mcp.pinMode(MCP_PIN_BTN_CC_PLUS, INPUT_PULLUP);
    canton->mcp.pinMode(MCP_PIN_INTER_DEV_2, INPUT_PULLUP);
    canton->mcp.pinMode(MCP_PIN_INTER_DEV_1, INPUT_PULLUP);

    // Bouton MANOEUVRE
    canton->mcp.pinMode(MCP_PIN_BTN_MANOEUVRE, INPUT_PULLUP);

    // LED Exploration (uniquement pour la découverte)
    canton->mcp.pinMode(MCP_PIN_LED_EXPLORATION, OUTPUT);

    // LED MANOEUVRE (reflète l’état uniquement en exploration)
    canton->mcp.pinMode(MCP_PIN_LED_MANOEUVRE, OUTPUT);

    // Mise à jour initiale LED MANOEUVRE (on est en exploration)
    canton->mcp.digitalWrite(MCP_PIN_LED_MANOEUVRE,
                             canton->modeManoeuvre() ? HIGH : LOW);

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
// process() — gestion boutons + reset logique + CAN + MANOEUVRE
// ---------------------------------------------------------------------------
void Exploration::process(void *p) // 🟢
{
    Canton *canton = (Canton *)p;
    bool ledAllumee = false;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    static bool lastManoeuvreBtn = false;

    auto clignoterLEDexploration = [&]()
    {
        canton->mcp.digitalWrite(MCP_PIN_LED_EXPLORATION,
                                 ledAllumee ? HIGH : LOW);
        ledAllumee = !ledAllumee;
    };

    auto eteindreLEDexploration = [&]()
    {
        canton->mcp.digitalWrite(MCP_PIN_LED_EXPLORATION, LOW);
    };

    auto majLEDmanoeuvre = [&]()
    {
        // LED MANOEUVRE = reflète l’état uniquement en exploration
        canton->mcp.digitalWrite(MCP_PIN_LED_MANOEUVRE,
                                 canton->modeManoeuvre() ? HIGH : LOW);
    };

    auto eteindreLEDmanoeuvre = [&]()
    {
        canton->mcp.digitalWrite(MCP_PIN_LED_MANOEUVRE, LOW);
    };

    auto btnPush = [&](uint8_t btnNum)
    {
        // Notification CAN vers la carte ERM : demande d’attribution d’ID
        CC_CAN::sendMsg(
            0,
            (uint16_t)Cmd_Exploration_CC::DEMANDE_ID,
            0,
            canton->ID(),
            UNUSED_ID,
            0);

        if (m_ID_satPeriph < 253)
        {
            CantonPeriph *np = canton->getCantonP(btnNum);
            if (!np)
            {
                np = new CantonPeriph;
                canton->setCantonP(btnNum, np);
            }

            // ID temporaire en attendant la réponse de l’ERM
            np->ID(15);

            clignoterLEDexploration();
            m_ID_satPeriph = UNUSED_ID;
        }
        else
        {
            clignoterLEDexploration();
        }
    };

    for (;;)
    {
        // Lecture boutons
        bool satMoins = !canton->mcp.digitalRead(MCP_PIN_BTN_CC_MOINS);
        bool satPlus = !canton->mcp.digitalRead(MCP_PIN_BTN_CC_PLUS);
        bool dev2 = !canton->mcp.digitalRead(MCP_PIN_INTER_DEV_2);
        bool dev1 = !canton->mcp.digitalRead(MCP_PIN_INTER_DEV_1);

        bool btnManoeuvre = !canton->mcp.digitalRead(MCP_PIN_BTN_MANOEUVRE);

        // Toggle MANOEUVRE
        if (btnManoeuvre && !lastManoeuvreBtn)
        {
            bool newState = !canton->modeManoeuvre();
            canton->setModeManoeuvre(newState);

            Settings::writeFile(canton);

            // Mise à jour LED MANOEUVRE (exploration uniquement)
            majLEDmanoeuvre();

            CC_LOG_INFO("[Exploration][Manoeuvre] Mode = %s\n",
                        newState ? "ACTIF" : "INACTIF");
        }
        lastManoeuvreBtn = btnManoeuvre;

        // LED MANOEUVRE = mise à jour continue en exploration
        majLEDmanoeuvre();

        // Gestion découverte
        m_btnState =
            (satMoins ? 0x01 : 0) |
            (satPlus ? 0x02 : 0) |
            (dev2 ? 0x04 : 0) |
            (dev1 ? 0x08 : 0);

        switch (m_btnState & 0x03)
        {
        case 0x01:
            btnPush(m_btnState >> 2);
            break;

        case 0x02:
            btnPush((m_btnState >> 2) + 4);
            break;

        case 0x03:
            // RESET LOGIQUE + nouvelle découverte
            for (uint8_t i = 0; i < cantonPsize; i++)
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

            // Réinitialisation des aiguilles
            for (uint8_t i = 0; i < aigSize; i++)
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
            for (uint8_t i = 0; i < signalSize; i++)
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
            clignoterLEDexploration();

            runExplorationPass(canton);
            break;

        default:
            eteindreLEDexploration();
            break;
        }

        // Passage en exploitation ?
        if (m_stopProcess)
        {
            // LED MANOEUVRE doit être OFF en exploitation
            eteindreLEDmanoeuvre();
            vTaskDelete(NULL);
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
    }
}

// ---------------------------------------------------------------------------
// createAigEtCibles() — 1ère passe au boot
// ---------------------------------------------------------------------------
void Exploration::createAigEtCibles(void *p) // 🟢
{
    Canton *canton = (Canton *)p;

    runExplorationPass(canton);

    vTaskDelete(NULL);
}
