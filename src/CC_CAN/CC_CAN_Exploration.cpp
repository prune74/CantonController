/*
 * CC_CAN_Exploration.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Commandes CAN du mode Exploration (0xC0–0xC1)
 *
 * Rôle :
 *   - CMD_EXPLORATION_ID_VOISIN :
 *         réception de l’ID d’un CC voisin
 *
 *   - CMD_EXPLORATION_UPDATE_MASQUE_AIG :
 *         mise à jour du masque d’aiguilles d’un voisin
 */

#include "CC_CAN.h"
#include "debug_cc.h"
#include "Exploration.h"
#include "CanMsg.h"
#include "Canton.h"
#include "Protocol.h"

/* ============================================================================
 * Handler global Exploration (appelé depuis CC_CAN.cpp)
 * ==========================================================================*/
void handleExplorationCommand(uint8_t commande,
                              const CanMsg &msg,
                              Canton *canton,
                              uint16_t idSatExpediteur)
{
    // Conversion explicite vers l’enum class CanCmd
    CanCmd cmd = static_cast<CanCmd>(commande);

    switch (cmd)
    {
    /* --------------------------------------------------------------------
     * CMD_EXPLORATION_ID_VOISIN — Réception de l’ID du CC voisin
     * ------------------------------------------------------------------ */
    case CanCmd::CMD_EXPLORATION_ID_VOISIN:
        Exploration::ID_satPeriph(idSatExpediteur);
        break;

    /* --------------------------------------------------------------------
     * CMD_EXPLORATION_UPDATE_MASQUE_AIG — Réception du masque d’aiguilles
     * --------------------------------------------------------------------
     * Réception du masque d’aiguilles envoyé par le CC voisin pendant
     * la phase d’exploration, puis mise à jour du CantonPeriph correspondant.
     * ------------------------------------------------------------------ */
    case CanCmd::CMD_EXPLORATION_UPDATE_MASQUE_AIG:
    {
        uint8_t masque = msg.data[0];

        for (uint8_t i = 0; i < cantonPsize; i++)
        {
            CantonPeriph *p = canton->getCantonP(i);
            if (p && p->ID() == idSatExpediteur)
            {
                p->masqueAig(masque);
            }
        }
        break;
    }

    default:
        break;
    }
}
