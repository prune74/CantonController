/*
 * CC_CAN_Supervision.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Supervision réseau (OFFLINE)
 */

#include "CC_CAN.h"
#include "Canton.h"
#include "debug_cc.h"
#include "CanMsg.h"
#include <Protocol.h>

/* ============================================================================
 * handleSupervisionCommand()
 * ==========================================================================*/
void handleSupervisionCommand(uint8_t commande,
                              const CanMsg &msg,
                              Canton *canton)
{

    switch ((Cmd_ERM_to_CC)commande)
    {
    /* --------------------------------------------------------------------
     * OFFLINE — Notification d’un CC hors ligne
     * ------------------------------------------------------------------ */
    case Cmd_ERM_to_CC::OFFLINE:
    {
        uint16_t offlineId = msg.data[0] | (msg.data[1] << 8);

        CC_LOG_WARN("[CAN][Supervision][CC] CC offline détecté : %u\n", offlineId);

        // Vérification locale de la topologie
        canton->checkTopoValidity(offlineId);
        break;
    }

    default:
        CC_LOG_WARN("[CAN][Supervision][CC] Commande supervision inconnue : 0x%X\n",
                    commande);
        break;
    }
}
