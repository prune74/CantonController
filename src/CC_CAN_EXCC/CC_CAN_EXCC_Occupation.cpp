/*
 * CC_CAN_EXCC_Occupation.cpp — Gestion Canton 2026 (CAN → EXCC)
 * ---------------------------------------------------------------------------
 * Transmission de l’occupation des cantons voisins vers l’EXCC via CAN.
 */

#include "CC_CAN_EXCC.h"
#include "Settings.h"
#include "Canton.h"
#include "debug_cc.h"

/* ============================================================================
 *  sendOccVoisinsDepuisEtatCourant()
 * ---------------------------------------------------------------------------
 *  Lit l’occupation SP1 / SM1 et envoie l’octet EA correspondant.
 * ==========================================================================*/
namespace CC_CAN_EXCC
{
    void sendOccVoisinsDepuisEtatCourant()
    {
        Canton *c = Settings::canton;

        uint8_t occSP1 = 0;
        uint8_t occSM1 = 0;

        CantonPeriph *sp1 = c->getCantonP(c->SP1_idx());
        CantonPeriph *sm1 = c->getCantonP(c->SM1_idx());

        if (sp1 && sp1->busy())
            occSP1 = 1;

        if (sm1 && sm1->busy())
            occSM1 = 1;

        uint8_t valeur = (occSP1 << 1) | occSM1;

        // Envoi CAN vers CC
        CC_CAN_EXCC::sendOccupationVoisins(c, valeur);

        CC_LOG_INFO("[EXCC][Occupation][CAN] EA envoyé : SP1=%u SM1=%u (valeur=%u)\n",
                    occSP1, occSM1, valeur);
    }
}