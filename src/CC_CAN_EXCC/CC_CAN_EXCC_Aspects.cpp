/*
 * CC_CAN_EXCC_Aspects.cpp — Gestion Canton 2026 (CAN → EXCC)
 * ---------------------------------------------------------------------------
 * Transmission des aspects SNCF vers l’EXCC via CAN.
 */

#include "CC_CAN_EXCC.h"
#include "Settings.h"
#include "SupervisionCanton.h"
#include "debug_cc.h"

/* ============================================================================
 *  sendAspectsDepuisEtatCourant()
 * ---------------------------------------------------------------------------
 *  Envoie les aspects SNCF calculés :
 *    → ASPECT_HORAIRE
 *    → ASPECT_ANTIHORAIRE
 * ==========================================================================*/
namespace CC_CAN_EXCC
{
    void sendAspectsDepuisEtatCourant()
    {
        Canton *canton = Settings::canton;

        // Calcul des aspects (logique métier dans SupervisionCanton)
        ExccAspect aspectHoraire      = mettreAJourAspectCanton(canton, 0);
        ExccAspect aspectAntiHoraire  = mettreAJourAspectCanton(canton, 1);

        // Transmission CAN vers EXCC
        CC_CAN_EXCC::sendAspectHoraire(canton, aspectHoraire);
        CC_CAN_EXCC::sendAspectAntiHoraire(canton, aspectAntiHoraire);

        CC_LOG_INFO("[EXCC][Aspects][CAN] Aspects envoyés : H=%u AH=%u\n",
                    static_cast<uint8_t>(aspectHoraire),
                    static_cast<uint8_t>(aspectAntiHoraire));
    }
}
