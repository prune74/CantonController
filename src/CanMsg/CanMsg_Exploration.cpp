/*
 * CC_CanMsg_Exploration.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Commandes CAN Exploration (0xC0–0xC1)
 *
 * Rôle :
 *   - utilisées UNIQUEMENT pendant le mode Exploration
 *   - permettent :
 *       0xC0 → identification du CC voisin
 *       0xC1 → réception du masque d’aiguilles d’un voisin
 *
 * IMPORTANT :
 *   - aucune logique métier
 *   - aucune logique ferroviaire
 *   - aucune logique d’accès ou de sécurité
 *
 * Ce module ne fait que décoder les trames CAN du mode Exploration.
 */

#include "CanMsg.h"
#include "debug_cc.h"

/* ============================================================================
 * Handler global Exploration
 * ==========================================================================*/
void handleExplorationCommand(uint8_t commande, const CANMessage &frameIn,
                              Canton *canton, uint16_t idSatExpediteur)
{
    switch (commande)
    {
        /* --------------------------------------------------------------------
         * 0xC0 — Réception de l’ID du CC voisin
         * --------------------------------------------------------------------
         * idSatExpediteur = ID du CC détecté via CAN
         * ------------------------------------------------------------------ */
        case 0xC0:
            Exploration::ID_satPeriph(idSatExpediteur);
            break;

        /* --------------------------------------------------------------------
         * 0xC1 — Masque aiguilles pendant Exploration
         * --------------------------------------------------------------------
         * frameIn.data[0] = masque d’aiguilles envoyé par le voisin
         * ------------------------------------------------------------------ */
        case 0xC1:
        {
            uint8_t masque = frameIn.data[0];

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
