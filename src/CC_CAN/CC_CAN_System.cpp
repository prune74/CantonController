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
 * ---------------------------------------------------------------------------
 * @param commande         → code CAN système
 * @param CanMsg &msg          → trame CAN reçue
 * @param canton           → canton local
 * @param idSatExpediteur  → ID du CC expéditeur (non utilisé ici)
 *
 * Ce handler ne modifie JAMAIS la topologie ni l’exploitation.
 * ==========================================================================*/
void handleSystemCommand(uint8_t commande,
                         const CanMsg &msg,
                         Canton *canton,
                         uint16_t idSatExpediteur)
{
    (void)idSatExpediteur; // non utilisé

    switch (commande)
    {
    /* --------------------------------------------------------------------
     * CMD_ERM_CC_TEST_BUS_REPLY — Réponse au test du bus CAN
     * --------------------------------------------------------------------
     * msg.data[0] = 1 → CC opérationnel
     * Le ERM peut alors marquer ce CC comme READY.
     * ------------------------------------------------------------------ */
    case CMD_ERM_CC_TEST_BUS_REPLY:
        if (msg.data[0])
            Settings::sMainReady(true);
        break;

    /* --------------------------------------------------------------------
     * CMD_ERM_CC_REQUEST_ID — Attribution d’ID
     * --------------------------------------------------------------------
     * Si le CC n’a pas d’ID (UNUSED_ID), le ERM lui en attribue un.
     * ------------------------------------------------------------------ */
    case CMD_ERM_CC_REQUEST_ID:
        if (canton->ID() == UNUSED_ID)
            canton->ID(msg.data[0]);
        break;

    /* --------------------------------------------------------------------
     * CMD_ERM_CC_RESTART_ALL — Reset ESP32
     * --------------------------------------------------------------------
     * Commande critique : reboot immédiat du CC.
     * ------------------------------------------------------------------ */
    case CMD_ERM_CC_RESTART_ALL:
        ESP.restart();
        break;

    /* --------------------------------------------------------------------
     * CMD_ERM_CC_WIFI_ON_OFF — Activation / désactivation du WiFi
     * --------------------------------------------------------------------
     * msg.data[0] = 0 → OFF
     * msg.data[0] = 1 → ON
     *
     * Après modification → sauvegarde → reboot.
     * ------------------------------------------------------------------ */
    case CMD_ERM_CC_WIFI_ON_OFF:
        Settings::wifiOn(msg.data[0]);
        Settings::writeFile(Settings::canton);
        delay(1000);
        ESP.restart();
        break;

    /* --------------------------------------------------------------------
     * CMD_ERM_CC_EXPLORATION_ON_OFF — Activation / désactivation Exploration
     * --------------------------------------------------------------------
     * Exploration = apprentissage automatique SP/SM.
     *
     * ON  → reboot pour entrer en mode Exploration
     * OFF → arrêt immédiat du processus Exploration
     * ------------------------------------------------------------------ */
    case CMD_ERM_CC_EXPLORATION_ON_OFF:
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
     * --------------------------------------------------------------------
     * Le ERM impose une sauvegarde immédiate.
     * ------------------------------------------------------------------ */
    case CMD_ERM_CC_SAVE_ALL:
#ifdef SAUV_BY_MAIN
        Settings::writeFile(Settings::canton);
#else
        CC_LOG("[CAN][System][CC] Sauvegarde automatique désactivée.\n");
#endif
        break;

    default:
        break;
    }
}
