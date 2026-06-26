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
 * ==========================================================================*/
void handleSupervisionCommand(uint8_t commande,
                              const CanMsg &msg,
                              Canton *canton)
{
    // Conversion vers enum class
    CanCmd cmd = static_cast<CanCmd>(commande);

    switch (cmd)
    {
    /* --------------------------------------------------------------------
     * CMD_ERM_CC_OFFLINE — Notification d’un CC hors ligne
     * ------------------------------------------------------------------ */
    case CanCmd::CMD_ERM_CC_OFFLINE:
    {
        uint16_t offlineId = msg.data[0] | (msg.data[1] << 8);

        CC_LOG_WARN("[CAN][Supervision][CC] CC offline détecté : %u\n", offlineId);

        // Vérification locale de la topologie
        canton->checkTopoValidity(offlineId);
        break;
    }

    default:
        CC_LOG_WARN("[CAN][Supervision][CC] Commande supervision inconnue : 0x%X\n",
                    static_cast<uint8_t>(commande));
        break;
    }
}
