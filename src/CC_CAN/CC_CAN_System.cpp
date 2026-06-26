/*
 * CC_CAN_System.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Commandes système (CMD_ERM_CC_TEST_BUS_REPLY → CMD_ERM_CC_SAVE_ALL)
 */

#include "CC_CAN.h"
#include "debug_cc.h"
#include "Settings.h"
#include "Exploration.h"
#include "CanMsg.h"
#include "Canton.h"

/* ============================================================================
 * handleSystemCommand()
 * ==========================================================================*/
void handleSystemCommand(uint8_t commande,
                         const CanMsg &msg,
                         Canton *canton,
                         uint16_t idSatExpediteur)
{
    (void)idSatExpediteur; // non utilisé

    // Conversion vers enum class
    CanCmd cmd = static_cast<CanCmd>(commande);

    switch (cmd)
    {
    /* --------------------------------------------------------------------
     * CMD_ERM_CC_TEST_BUS_REPLY — Réponse au test du bus CAN
     * ------------------------------------------------------------------ */
    case CanCmd::CMD_ERM_CC_TEST_BUS_REPLY:
        if (msg.data[0])
            Settings::sMainReady(true);
        break;

    /* --------------------------------------------------------------------
     * CMD_ERM_CC_REQUEST_ID — Attribution d’ID
     * ------------------------------------------------------------------ */
    case CanCmd::CMD_ERM_CC_REQUEST_ID:
        if (canton->ID() == UNUSED_ID)
            canton->ID(msg.data[0]);
        break;

    /* --------------------------------------------------------------------
     * CMD_ERM_CC_RESTART_ALL — Reset ESP32
     * ------------------------------------------------------------------ */
    case CanCmd::CMD_ERM_CC_RESTART_ALL:
        ESP.restart();
        break;

    /* --------------------------------------------------------------------
     * CMD_ERM_CC_WIFI_ON_OFF — Activation / désactivation du WiFi
     * ------------------------------------------------------------------ */
    case CanCmd::CMD_ERM_CC_WIFI_ON_OFF:
        Settings::wifiOn(msg.data[0]);
        Settings::writeFile(Settings::canton);
        delay(1000);
        ESP.restart();
        break;

    /* --------------------------------------------------------------------
     * CMD_ERM_CC_EXPLORATION_ON_OFF — Activation / désactivation Exploration
     * ------------------------------------------------------------------ */
    case CanCmd::CMD_ERM_CC_EXPLORATION_ON_OFF:
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
     * CMD_ERM_CC_SAVE_ALL — Sauvegarde settings.json
     * ------------------------------------------------------------------ */
    case CanCmd::CMD_ERM_CC_SAVE_ALL:
#ifdef SAUV_DEPUIS_ERM
        Settings::writeFile(Settings::canton);
#else
        CC_LOG("[CAN][System][CC] Sauvegarde automatique désactivée.\n");
#endif
        break;

    default:
        break;
    }
}
