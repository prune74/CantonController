/*
 * PilotageDistribue.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Pilotage distribué des locomotives selon l’aspect LOCAL du canton.
 */

#include "PilotageDistribue.h"
#include "SensEnum.h"
#include <Protocol.h>
#include "Canton.h"
#include "debug_cc.h"

// ---------------------------------------------------------------------------
// Pilotage distribué principal
// ---------------------------------------------------------------------------
void executerPilotageDistribue(Canton *canton)
{
    if (!canton)
        return;

    Loco *loco = canton->getLoco();
    if (!loco)
        return;

    // -----------------------------------------------------------------------
    // 1) Déterminer l’aspect LOCAL selon le sens de marche
    // -----------------------------------------------------------------------
    SensDeMarche sens = loco->sens();
    ExccAspect aspectCommande = ExccAspect::ASPECT_CARRE;

    if (sens == SensHoraire)
    {
        aspectCommande = canton->aspectLocal(SensHoraire);
        CC_LOG_TRACE("[PilotageDistribue][CC] Sens H → aspect local(H)=%u\n",
                     static_cast<uint8_t>(aspectCommande));
    }
    else if (sens == SensAntiHoraire)
    {
        aspectCommande = canton->aspectLocal(SensAntiHoraire);
        CC_LOG_TRACE("[PilotageDistribue][CC] Sens AH → aspect local(AH)=%u\n",
                     static_cast<uint8_t>(aspectCommande));
    }
    else
    {
        CC_LOG_WARN("[PilotageDistribue][CC] Sens inconnu → pilotage ignoré\n");
        return;
    }

    // -----------------------------------------------------------------------
    // 2) Appliquer la vitesse correspondant à l’aspect LOCAL
    // -----------------------------------------------------------------------
    switch (aspectCommande)
    {
    case ExccAspect::ASPECT_CARRE:
    case ExccAspect::ASPECT_CARRE_VIOLET:
        CC_LOG_INFO("[PilotageDistribue][CC] Carré → arrêt\n");
        loco->speed(0);
        break;

    case ExccAspect::ASPECT_SEMAPHORE:
    case ExccAspect::ASPECT_AVERTISSEMENT:
        CC_LOG_INFO("[PilotageDistribue][CC] Avertissement → %u\n",
                    loco->vitesseAvertissement);
        loco->speed(loco->vitesseAvertissement);
        break;

    case ExccAspect::ASPECT_RALENTISSEMENT_30:
        CC_LOG_INFO("[PilotageDistribue][CC] Ralentissement 30 → %u\n",
                    loco->vitesseRalentissement30);
        loco->speed(loco->vitesseRalentissement30);
        break;

    case ExccAspect::ASPECT_RALENTISSEMENT_60:
        CC_LOG_INFO("[PilotageDistribue][CC] Ralentissement 60 → %u\n",
                    loco->vitesseRalentissement60);
        loco->speed(loco->vitesseRalentissement60);
        break;

    case ExccAspect::ASPECT_RAPPEL_30:
        CC_LOG_INFO("[PilotageDistribue][CC] Rappel 30 → %u\n",
                    loco->vitesseRappel30);
        loco->speed(loco->vitesseRappel30);
        break;

    case ExccAspect::ASPECT_RAPPEL_60:
        CC_LOG_INFO("[PilotageDistribue][CC] Rappel 60 → %u\n",
                    loco->vitesseRappel60);
        loco->speed(loco->vitesseRappel60);
        break;

    case ExccAspect::ASPECT_VOIE_LIBRE:
        CC_LOG_INFO("[PilotageDistribue][CC] Voie libre → %u\n",
                    loco->vitesseVoieLibre);
        loco->speed(loco->vitesseVoieLibre);
        break;

    case ExccAspect::ASPECT_MANOEUVRE:
        CC_LOG_INFO("[PilotageDistribue][CC] Manoeuvre → %u\n",
                    loco->vitesseManoeuvre);
        loco->speed(loco->vitesseManoeuvre);
        break;

    case ExccAspect::ASPECT_MASQUE:
        CC_LOG_INFO("[PilotageDistribue][CC] Aspect masqué → %u\n",
                    loco->vitesseAvertissement);
        loco->speed(loco->vitesseAvertissement);
        break;

    default:
        CC_LOG_WARN("[PilotageDistribue][CC] Aspect inconnu (%u) → arrêt sécurité\n",
                    static_cast<uint8_t>(aspectCommande));
        loco->speed(0);
        break;
    }
}
