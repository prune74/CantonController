/*
 * CC_CAN_EXCC_FeuxDirection.cpp — Gestion Canton 2026 (CAN → EXCC)
 * ---------------------------------------------------------------------------
 * Transmission des feux directionnels vers l’EXCC via CAN.
 *
 * Commandes :
 *   - DIRECTION_HORAIRE : feu directionnel horaire
 *   - DIRECTION_ANTIHORAIRE : feu directionnel anti‑horaire
 *
 * Rôle :
 *   - mettre à jour les feux directionnels (logique métier dans Canton_FeuxDirection)
 *   - transmettre les codes calculés (0..4) aux EXCC
 */

#include "CC_CAN_EXCC.h"
#include "Settings.h"
#include "Canton.h"
#include "debug_cc.h"

/* ============================================================================
 *  sendFeuxDepuisEtatCourant()
 * ---------------------------------------------------------------------------
 *  Met à jour les feux directionnels dans le Canton puis envoie :
 *    → DIRECTION_HORAIRE (horaire)
 *    → DIRECTION_ANTIHORAIRE (anti‑horaire)
 * ==========================================================================*/
namespace CC_CAN_EXCC
{
    void sendFeuxDepuisEtatCourant()
    {
        Canton *c = Settings::canton;

        // Mise à jour logique
        c->updateFeuDirection(SensHoraire);
        c->updateFeuDirection(SensAntiHoraire);

        // Lecture des codes (0..4)
        uint8_t codeH = c->getFeuDirection(SensHoraire);
        uint8_t codeAH = c->getFeuDirection(SensAntiHoraire);

        // Transmission CAN vers EXCC
        CC_CAN_EXCC::sendFeuDirectionHoraire(c, codeH);
        CC_CAN_EXCC::sendFeuDirectionAntiHoraire(c, codeAH);

        CC_LOG_INFO("[EXCC][Feux][CAN] Feux direction envoyés : H=%u AH=%u\n",
                    codeH, codeAH);
    }
}