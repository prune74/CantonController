/*
 * CanMsg_System.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Commandes système (CMD_SAT_TEST_BUS_REPLY → CMD_SAVE_ALL)
 *
 * Rôle :
 *   - tester le bus CAN
 *   - attribuer un ID au CC
 *   - activer/désactiver WiFi
 *   - activer/désactiver Exploration
 *   - sauvegarder settings.json
 *
 * IMPORTANT :
 *   - aucune logique ferroviaire
 *   - aucune logique topologique
 *   - aucune logique d’exploitation
 *
 * Ce module agit UNIQUEMENT sur les paramètres système.
 */

#include "CanMsg.h"
#include "debug_cc.h"

/* ============================================================================
 * handleSystemCommand()
 * ---------------------------------------------------------------------------
 * @param commande         → code CAN système
 * @param frameIn          → trame CAN reçue
 * @param canton           → canton local
 * @param idSatExpediteur  → ID du CC expéditeur (non utilisé ici)
 *
 * Ce handler ne modifie JAMAIS la topologie ni l’exploitation.
 * ==========================================================================*/
void handleSystemCommand(uint8_t commande, const CANMessage &frameIn,
                         Canton *canton, uint16_t idSatExpediteur)
{
    (void)idSatExpediteur; // non utilisé

    switch (commande)
    {
        /* --------------------------------------------------------------------
         * CMD_SAT_TEST_BUS_REPLY — Réponse au test du bus CAN
         * --------------------------------------------------------------------
         * frameIn.data[0] = 1 → CC opérationnel
         * Le Main peut alors marquer ce CC comme READY.
         * ------------------------------------------------------------------ */
        case CMD_SAT_TEST_BUS_REPLY:
            if (frameIn.data[0])
                Settings::sMainReady(true);
            break;

        /* --------------------------------------------------------------------
         * CMD_SAT_REQUEST_ID_REPLY — Attribution d’ID
         * --------------------------------------------------------------------
         * Si le CC n’a pas d’ID (UNUSED_ID), le Main lui en attribue un.
         * ------------------------------------------------------------------ */
        case CMD_SAT_REQUEST_ID_REPLY:
            if (canton->ID() == UNUSED_ID)
                canton->ID(frameIn.data[0]);
            break;

        /* --------------------------------------------------------------------
         * CMD_RESTART_ALL — Reset ESP32
         * --------------------------------------------------------------------
         * Commande critique : reboot immédiat du CC.
         * ------------------------------------------------------------------ */
        case CMD_RESTART_ALL:
            ESP.restart();
            break;

        /* --------------------------------------------------------------------
         * CMD_WIFI_ON_OFF — Activation / désactivation du WiFi
         * --------------------------------------------------------------------
         * frameIn.data[0] = 0 → OFF
         * frameIn.data[0] = 1 → ON
         *
         * Après modification → sauvegarde → reboot.
         * ------------------------------------------------------------------ */
        case CMD_WIFI_ON_OFF:
            Settings::wifiOn(frameIn.data[0]);
            Settings::writeFile(Settings::canton);
            delay(1000);
            ESP.restart();
            break;

        /* --------------------------------------------------------------------
         * CMD_EXPLORATION_ON_OFF — Activation / désactivation Exploration
         * --------------------------------------------------------------------
         * Exploration = apprentissage automatique SP/SM.
         *
         * ON  → reboot pour entrer en mode Exploration
         * OFF → arrêt immédiat du processus Exploration
         * ------------------------------------------------------------------ */
        case CMD_EXPLORATION_ON_OFF:
            if (frameIn.data[0])
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
         * CMD_SAVE_ALL — Sauvegarde settings.json
         * --------------------------------------------------------------------
         * Le Main impose une sauvegarde immédiate.
         * ------------------------------------------------------------------ */
        case CMD_SAVE_ALL:
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
