/*
 * CC_CAN_Mesure.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Envoi de la vitesse mesurée (loco + vitesse) vers l’ERM
 */

#include "CC_CAN.h"
#include "Canton.h"
#include "debug_cc.h"
#include "CanMsg.h"
#include "CanID.h"
#include "CanBus.h"

/* ============================================================================
 * handleMesureCommand()
 * ==========================================================================*/
void handleMesureCommand(uint8_t commande,
                         Canton *canton)
{
    switch ((Cmd_CC_to_ERM)commande)
    {
    /* --------------------------------------------------------------------
     * MESURE_VITESSE — Envoi vitesse mesurée vers l’ERM
     * ------------------------------------------------------------------ */
    case Cmd_CC_to_ERM::MESURE_VITESSE:
    {
        if (!canton->mesureVitesseDisponible())
            return;

        uint16_t loco = canton->locoMesuree();
        float v = canton->vitesseMesuree();
        uint16_t v1000 = (uint16_t)(v * 1000.0f);

        CC_LOG_INFO("[CAN][Mesure][CC] Envoi vitesse %.3f pour loco %u\n",
                    v, loco);

        CC_CAN::sendMsg(
            0,                                      // prio
            (uint8_t)Cmd_CC_to_ERM::MESURE_VITESSE, // commande
            0,                                      // resp
            canton->ID(),                           // ID CC source
            loco >> 8,                              // d0
            loco & 0xFF,                            // d1
            v1000 >> 8,                             // d2
            v1000 & 0xFF                            // d3
        );

        canton->clearMesureVitesse();
        break;
    }
    
    default:
        CC_LOG_WARN("[CAN][Mesure][CC] Commande inconnue : 0x%X\n",
                    commande);
        break;
    }
}
