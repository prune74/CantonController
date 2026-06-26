/*
 * CC_CAN_Supervision.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Supervision réseau (CMD_ERM_CC_OFFLINE)
 */

#include "CC_CAN.h"
#include "Canton.h"
#include "debug_cc.h"
#include "CanMsg.h"

/* ============================================================================
 * handleSupervisionCommand()
 * ---------------------------------------------------------------------------
 * @param commande  → code CAN supervision (CMD_ERM_CC_OFFLINE)
 * @param frameIn   → trame CAN reçue
 * @param canton    → canton local
 *
 * Ce handler traite uniquement les messages envoyés par ERM
 * concernant l’état du réseau (ex : CC offline).
 * ==========================================================================*/
void handleSupervisionCommand(uint8_t commande,
                              const CanMsg &msg,
                              Canton *canton)
{
    switch (commande)
    {
    /* --------------------------------------------------------------------
     * CMD_ERM_CC_OFFLINE — Notification d’un CC hors ligne
     * --------------------------------------------------------------------
     * frameIn.data :
     *   [0] offlineId_low
     *   [1] offlineId_high
     *
     * Logique :
     *   - ERM détecte un CC offline
     *   - ERM envoie son ID à tous les CC
     *   - chaque CC vérifie localement si cet ID est un voisin
     * ------------------------------------------------------------------ */
    case CMD_ERM_CC_OFFLINE:
    {
        uint16_t offlineId = msg.data[0] | (msg.data[1] << 8);

        CC_LOG_WARN("[CAN][Supervision][CC] CC offline détecté : %u\n", offlineId);

        // Vérification locale de la topologie
        canton->checkTopoValidity(offlineId);
        break;
    }

    default:
        CC_LOG_WARN("[CAN][Supervision][CC] Commande supervision inconnue : 0x%X\n", commande);
        break;
    }
}
