/*
 * CC_CAN_Supervision.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Gestion des messages CAN de supervision réseau.
 *
 * Rôle :
 *   - recevoir l’ID d’un CC offline envoyé par ERM
 *   - vérifier localement si cet ID est un voisin du canton
 *   - mettre à jour l’état topologique local
 *   - déclencher la LED “topologie invalide”
 *
 * IMPORTANT :
 *   • aucune logique ferroviaire
 *   • aucune logique d’exploitation
 *   • aucune logique d’exploration
 *
 * Ce module est dédié EXCLUSIVEMENT à la supervision réseau.
 */

#include "CC_CAN.h"
#include "Canton.h"
#include "debug_cc.h"

/* ============================================================================
 * handleSupervisionCommand()
 * ---------------------------------------------------------------------------
 * @param commande  → code CAN supervision (CMD_CC_OFFLINE)
 * @param frameIn   → trame CAN reçue
 * @param canton    → canton local
 *
 * Ce handler traite uniquement les messages envoyés par ERM
 * concernant l’état du réseau (ex : CC offline).
 * ==========================================================================*/
void handleSupervisionCommand(uint8_t commande, const CANMessage &frameIn, Canton *canton)
{
    switch (commande)
    {
    /* --------------------------------------------------------------------
     * CMD_CC_OFFLINE — Notification d’un CC hors ligne
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
    case CMD_CC_OFFLINE:
    {
        uint16_t offlineId = frameIn.data[0] | (frameIn.data[1] << 8);

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
