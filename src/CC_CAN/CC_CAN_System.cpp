/*
 * CC_CAN_System.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Commandes système ERM → CC
 */

#include "CC_CAN.h"
#include "CC_CAN_EXCC.h"
#include "debug_cc.h"
#include "Settings.h"
#include "Exploration.h"
#include "CanMsg.h"
#include "Canton.h"
#include <Protocol.h>

/* ============================================================================
 * handleSystemCommand()
 * ==========================================================================*/
void handleSystemCommand(uint8_t commande,
                         const CanMsg &msg,
                         Canton *canton,
                         uint16_t idCCExpediteur)
{
    (void)idCCExpediteur; // non utilisé

    switch ((Cmd_ERM_to_CC)commande)
    {
    /* --------------------------------------------------------------------
     * TEST_BUS_REPLY — Réponse au test du bus CAN
     * ------------------------------------------------------------------ */
    case Cmd_ERM_to_CC::TEST_BUS_REPLY:
        if (msg.data[0])
            Settings::sMainReady(true);
        break;

    /* --------------------------------------------------------------------
     * REQUEST_ID_REPLY — Attribution d’ID
     * ------------------------------------------------------------------ */
    case Cmd_ERM_to_CC::REQUEST_ID_REPLY:
        if (canton->ID() == UNUSED_ID)
            canton->ID(msg.data[0]);
        break;

    /* --------------------------------------------------------------------
     * RESTART_ALL — Reset ESP32
     * ------------------------------------------------------------------ */
    case Cmd_ERM_to_CC::RESTART_ALL:
        ESP.restart();
        break;

    /* --------------------------------------------------------------------
     * WIFI_ON_OFF — Activation / désactivation du WiFi
     * ------------------------------------------------------------------ */
    case Cmd_ERM_to_CC::WIFI_ON_OFF:
        Settings::wifiOn(msg.data[0]);
        Settings::writeFile(Settings::canton);
        delay(1000);
        ESP.restart();
        break;

    /* --------------------------------------------------------------------
     * EXPLORATION_ON_OFF — Activation / désactivation Exploration
     * ------------------------------------------------------------------ */
    case Cmd_ERM_to_CC::EXPLORATION_ON_OFF:
        if (msg.data[0])
        {
            Settings::explorationOn(true);
            Settings::writeFile(Settings::canton);
            delay(1000);
            ESP.restart();
        }
        else
        {
            Settings::explorationOn(false);
            Exploration::stopProcess(true);
            Settings::writeFile(Settings::canton);
        }
        break;

    /* --------------------------------------------------------------------
     * SAVE_ALL — Sauvegarde settings.json
     * ------------------------------------------------------------------ */
    case Cmd_ERM_to_CC::SAVE_ALL:
#ifdef SAUV_DEPUIS_ERM
        Settings::writeFile(Settings::canton);
#else
        CC_LOG("[CAN][System][CC] Sauvegarde automatique désactivée.\n");
#endif
        break;

    /* --------------------------------------------------------------------
     * SET_PROFILE — Profil de voie (12V / 15V)
     * ------------------------------------------------------------------ */
    case Cmd_ERM_to_CC::SET_PROFILE:
    {
        bool profile = (msg.data[0] == 1);

        Settings::TRACK_PROFILE = profile;
        Settings::writeFile(Settings::canton);

        CC_LOG_INFO("[CAN][System][CC] SET_PROFILE : %s\n",
                    profile ? "15V (HO)" : "12V (N)");

        CC_CAN_EXCC::sendTrackProfileDepuisSettings();

        break;
    }

    default:
        break;
    }
}
