/*
 * CC_CAN_EXCC_Aiguilles.cpp — Gestion Canton 2026 (CAN → EXCC)
 * ---------------------------------------------------------------------------
 * Envoi des mouvements réels d’aiguilles vers l’EXCC via CAN (commande CMD_EXCC_CC_POSITION_AIGUILLE).
 */

#include "CC_CAN_EXCC.h"
#include "Settings.h"
#include "Canton.h"
#include "debug_cc.h"

/* ============================================================================
 *  sendAiguillesDepuisEtatCourant()
 * ---------------------------------------------------------------------------
 *  Envoie CMD_EXCC_CC_POSITION_AIGUILLE pour chaque aiguille logique active.
 * ==========================================================================*/
namespace CC_CAN_EXCC
{

    void sendAiguillesDepuisEtatCourant()
    {
        Canton *canton = Settings::canton;

        for (uint8_t idx = 0; idx < aigSize; ++idx)
        {
            Aig *aig = canton->getAig(idx);
            if (!aig)
                continue;

            uint8_t direction = aig->estDroit() ? 0 : 1;

            // Envoi CAN → EXCC
            CC_CAN_EXCC::sendPositionAiguille(canton, idx, direction);

            CC_LOG_INFO("[TopoCAN][CC] CMD_EXCC_CC_POSITION_AIGUILLE → servo=%u direction=%u (estDroit=%u)\n",
                        idx, direction, aig->estDroit());
        }

        CC_LOG_INFO("[TopoCAN][CC] Aiguilles renvoyées (CAN) après reboot EXCC\n");
    }

}
