/*
 * SatTopo_UART_FeuxDirection.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Transmission des feux directionnels vers les EXCC via UART.
 *
 * OpCodes :
 *   - E8 : feu directionnel horaire
 *   - E9 : feu directionnel anti‑horaire
 *
 * Rôle :
 *   - mettre à jour les feux directionnels (logique métier dans Canton_FeuxDirection)
 *   - transmettre les codes calculés (0..4) aux EXCC
 *
 * IMPORTANT :
 *   - aucune logique métier ici
 *   - aucun calcul topologique
 *   - aucune lecture d’aiguilles ou d’occupation
 *
 * Ce module se contente d’envoyer :
 *      “Voici le code directionnel H/AH calculé pour ce canton”
 */

#include "SatTopologieUART.h"
#include "Config.h"
#include "Exploration_Protocol.h"
#include "debug_cc.h"

#include "Settings.h"
#include "FeuxDirection.h"
#include "Canton.h"

extern HardwareSerial Serial1;

/* ============================================================================
 *  envoyerFeuxDepuisEtatCourant()
 * ---------------------------------------------------------------------------
 *  Met à jour les feux directionnels dans le Canton puis envoie :
 *    → E8 (horaire)
 *    → E9 (anti‑horaire)
 * ==========================================================================*/
void envoyerFeuxDepuisEtatCourant()
{
    // -----------------------------------------------------------------------
    // Mise à jour logique (calcul métier dans Canton_FeuxDirection)
    // -----------------------------------------------------------------------
    Settings::canton->updateFeuDirection(SensHoraire);
    Settings::canton->updateFeuDirection(SensAntiHoraire);

    // -----------------------------------------------------------------------
    // Lecture des codes (0..4)
    // -----------------------------------------------------------------------
    uint8_t codeH  = Settings::canton->getFeuDirection(SensHoraire);
    uint8_t codeAH = Settings::canton->getFeuDirection(SensAntiHoraire);

    // -----------------------------------------------------------------------
    // Transmission UART vers EXCC
    // -----------------------------------------------------------------------
    envoyerFeuDirectionHoraire(codeH);
    envoyerFeuDirectionAntiHoraire(codeAH);

    CC_LOG_INFO("[TopoUART][CC] Feux direction envoyés : H=%u AH=%u\n",
                codeH, codeAH);
}

/* ============================================================================
 *  envoyerFeuDirectionHoraire() — opcode E8
 * ==========================================================================*/
void envoyerFeuDirectionHoraire(uint8_t code)
{
    CC_LOG_TRACE("[TopoUART][CC] Feu direction horaire (E8) = %u\n", code);

    Serial1.write(PROTO_SYNC_BYTE);
    Serial1.write(PROTO_E8_DIRECTION_HORAIRE);
    Serial1.write(code);
}

/* ============================================================================
 *  envoyerFeuDirectionAntiHoraire() — opcode E9
 * ==========================================================================*/
void envoyerFeuDirectionAntiHoraire(uint8_t code)
{
    CC_LOG_TRACE("[TopoUART][CC] Feu direction anti-horaire (E9) = %u\n", code);

    Serial1.write(PROTO_SYNC_BYTE);
    Serial1.write(PROTO_E9_DIRECTION_ANTIHORAIRE);
    Serial1.write(code);
}
