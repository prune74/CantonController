/*
 * SatTopo_UART_Aspects.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Transmission des aspects SNCF vers les EXCC via UART.
 *
 * OpCodes :
 *   - E6 : aspect horaire
 *   - E7 : aspect anti‑horaire
 *
 * Rôle :
 *   - appeler mettreAJourAspectCanton() pour chaque sens (H / AH)
 *   - transmettre les aspects calculés aux EXCC
 *
 * IMPORTANT :
 *   - aucune logique métier ici
 *   - aucun calcul d’aspect
 *   - aucune lecture de topologie
 *
 * Toute la logique métier est dans SupervisionCanton.cpp.
 */

#include "SatTopologieUART.h"
#include "Config.h"
#include "Exploration_Protocol.h"
#include "debug_cc.h"

#include "Settings.h"
#include "SupervisionCanton.h"

extern HardwareSerial Serial1;

/* ============================================================================
 *  envoyerAspectsDepuisEtatCourant()
 * ---------------------------------------------------------------------------
 *  Envoie les aspects SNCF calculés :
 *    → E6 (horaire)
 *    → E7 (anti‑horaire)
 * ==========================================================================*/
void envoyerAspectsDepuisEtatCourant() // 🟢
{
    // -----------------------------------------------------------------------
    // Calcul des aspects (logique métier dans SupervisionCanton)
    // -----------------------------------------------------------------------
    ExccAspect aspectHoraire =
        mettreAJourAspectCanton(Settings::canton, 0);

    ExccAspect aspectAntiHoraire =
        mettreAJourAspectCanton(Settings::canton, 1);

    // -----------------------------------------------------------------------
    // Transmission UART vers EXCC
    // -----------------------------------------------------------------------
    envoyerAspectSignalHoraire(aspectHoraire);
    envoyerAspectSignalAntiHoraire(aspectAntiHoraire);

    CC_LOG_INFO("[TopoUART][CC] Aspects envoyés : H=%u AH=%u\n",
                aspectHoraire, aspectAntiHoraire);
}
