/*
 * CC_CAN_Exploration.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Commandes CAN du mode Exploration (0xC0–0xC1)
 */

#include "CC_CAN.h"
#include "debug_cc.h"
#include "Exploration.h"
#include "CanMsg.h"
#include "Canton.h"
#include <Protocol.h>

/* ============================================================================
 * Handler global Exploration (appelé depuis CC_CAN.cpp)
 * ==========================================================================*/
void handleExplorationCommand(uint8_t commande,
                              const CanMsg &msg,
                              Canton *canton,
                              uint16_t idCCExpediteur)
{
    // Conversion vers enum class dédié à l’exploration
    Cmd_Exploration_CC cmd = static_cast<Cmd_Exploration_CC>(commande);

    switch (cmd)
    {
    /* --------------------------------------------------------------------
     * ID_VOISIN — Réception de l’ID du CC voisin
     * ------------------------------------------------------------------ */
    case Cmd_Exploration_CC::ID_VOISIN:
        Exploration::ID_CCPeriph(idCCExpediteur);
        break;

    /* --------------------------------------------------------------------
     * UPDATE_MASQUE_AIG — Réception du masque d’aiguilles
     * ------------------------------------------------------------------ */
    case Cmd_Exploration_CC::UPDATE_MASQUE_AIG:
    {
        uint8_t masque = msg.data[0];

        for (uint8_t i = 0; i < cantonPsize; i++)
        {
            CantonPeriph *p = canton->getCantonP(i);
            if (p && p->ID() == idCCExpediteur)
            {
                p->masqueAigTopo(masque);
            }
        }
        break;
    }

    default:
        // Commande exploration inconnue → silencieux
        break;
    }
}
