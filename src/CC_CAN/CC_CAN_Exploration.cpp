/*
 * CC_CC_CAN_Exploration.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Commandes CAN du mode Exploration (0xC0–0xC1)
 *
 * Rôle :
 *   - 0xC0 : réception de l’ID d’un CC voisin détecté via CAN
 *   - 0xC1 : mise à jour du masque d’aiguilles d’un voisin
 *
 * IMPORTANT 2026 :
 *   - le masque est stocké uniquement dans CantonPeriph
 *   - aucun masque n’est maintenu dans la classe Canton
 *   - aucune logique métier ici : simple relais d’informations
 */

#include "CC_CAN.h"
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
     * ------------------------------------------------------------------ */
    case 0xC0:
        Exploration::ID_satPeriph(idSatExpediteur);
        break;

    /* --------------------------------------------------------------------
     * 0xC1 — Réception du masque d’aiguilles d’un voisin
     * --------------------------------------------------------------------
     * frameIn.data[0] = masque d’aiguilles du CC voisin
     *
     * NOTE 2026 :
     *   - ce masque est stocké dans CantonPeriph
     *   - il sert à détecter un voisin dangereux
     *   - il n’est JAMAIS stocké dans Canton
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
