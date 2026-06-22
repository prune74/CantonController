/*
 * PilotageDistribue.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Pilotage distribué des locomotives selon l’aspect reçu du canton voisin.
 *
 * Rôle :
 *   - déterminer le voisin pertinent selon le sens de marche
 *   - lire l’aspect SNCF reçu (aspectRecu[H/AH])
 *   - appliquer la vitesse correspondante à la locomotive
 *
 * Ce module ne calcule aucun aspect : il applique simplement la logique
 * déterminée par SupervisionCanton et transmise via CantonPeriph.
 */

#include "PilotageDistribue.h"
#include "SensEnum.h"
#include "Exploration_Protocol.h"
#include "Canton.h"
#include "debug_cc.h"

// ---------------------------------------------------------------------------
// Pilotage distribué principal
// ---------------------------------------------------------------------------
void executerPilotageDistribue(Canton *canton) // 🟢
{
    if (!canton)
        return;

    Loco *loco = canton->getLoco();
    if (!loco)
        return;

    ExccAspect aspectCommande = ASPECT_CARRE;
    CantonPeriph *voisin = nullptr;

    // -----------------------------------------------------------------------
    // 1) Déterminer le voisin pertinent selon le sens de marche
    // -----------------------------------------------------------------------
    SensDeMarche sens = loco->sens();

    switch (sens)
    {
    case SensHoraire:
    {
        uint8_t idx = canton->SP1_idx();
        voisin = canton->getCantonP(idx);

        if (voisin)
            aspectCommande = static_cast<ExccAspect>(voisin->aspectRecu[0]);

        CC_LOG_TRACE("[PilotageDistribue][CC] Sens H → SP1=%u aspect=%u\n",
                     idx, aspectCommande);
        break;
    }

    case SensAntiHoraire:
    {
        uint8_t idx = canton->SM1_idx();
        voisin = canton->getCantonP(idx);

        if (voisin)
            aspectCommande = static_cast<ExccAspect>(voisin->aspectRecu[1]);

        CC_LOG_TRACE("[PilotageDistribue][CC] Sens AH → SM1=%u aspect=%u\n",
                     idx, aspectCommande);
        break;
    }

    default:
        CC_LOG_WARN("[PilotageDistribue][CC] Sens inconnu → pilotage ignoré\n");
        return;
    }

    // -----------------------------------------------------------------------
    // 2) Appliquer la vitesse correspondant à l’aspect reçu
    // -----------------------------------------------------------------------
    switch (aspectCommande)
    {
    case ASPECT_CARRE:
    case ASPECT_CARRE_VIOLET:
        CC_LOG_INFO("[PilotageDistribue][CC] Carré (rouge/violet) → arrêt\n");
        loco->speed(0);
        break;

    case ASPECT_SEMAPHORE:
    case ASPECT_AVERTISSEMENT:
        CC_LOG_INFO("[PilotageDistribue][CC] Avertissement → %u\n",
                    loco->vitesseAvertissement);
        loco->speed(loco->vitesseAvertissement);
        break;

    case ASPECT_RALENTISSEMENT_30:
        CC_LOG_INFO("[PilotageDistribue][CC] Ralentissement 30 → %u\n",
                    loco->vitesseRalentissement30);
        loco->speed(loco->vitesseRalentissement30);
        break;

    case ASPECT_RALENTISSEMENT_60:
        CC_LOG_INFO("[PilotageDistribue][CC] Ralentissement 60 → %u\n",
                    loco->vitesseRalentissement60);
        loco->speed(loco->vitesseRalentissement60);
        break;

    case ASPECT_RAPPEL_30:
        CC_LOG_INFO("[PilotageDistribue][CC] Rappel 30 → %u\n",
                    loco->vitesseRappel30);
        loco->speed(loco->vitesseRappel30);
        break;

    case ASPECT_RAPPEL_60:
        CC_LOG_INFO("[PilotageDistribue][CC] Rappel 60 → %u\n",
                    loco->vitesseRappel60);
        loco->speed(loco->vitesseRappel60);
        break;

    case ASPECT_VOIE_LIBRE:
        CC_LOG_INFO("[PilotageDistribue][CC] Voie libre → %u\n",
                    loco->vitesseVoieLibre);
        loco->speed(loco->vitesseVoieLibre);
        break;

    case ASPECT_MANOEUVRE:
        CC_LOG_INFO("[PilotageDistribue][CC] Manoeuvre → %u\n",
                    loco->vitesseManoeuvre);
        loco->speed(loco->vitesseManoeuvre);
        break;

    case ASPECT_MASQUE:
        CC_LOG_INFO("[PilotageDistribue][CC] Aspect masqué → %u\n",
                    loco->vitesseAvertissement);
        loco->speed(loco->vitesseAvertissement);
        break;

    default:
        CC_LOG_WARN("[PilotageDistribue][CC] Aspect inconnu (%u) → arrêt sécurité\n",
                    aspectCommande);
        loco->speed(0);
        break;
    }
}
