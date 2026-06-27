/*
 * PilotageDistribue.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Pilotage distribué des locomotives selon l’aspect reçu du canton voisin.
 */

#include "PilotageDistribue.h"
#include "SensEnum.h"
#include "Protocol.h"
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

    ExccAspect aspectCommande = ExccAspect::ASPECT_CARRE;
    CantonPeriph *voisin = nullptr;

    // -----------------------------------------------------------------------
    // 1) Déterminer le voisin pertinent selon le sens de marche
    // -----------------------------------------------------------------------
    SensDeMarche sens = loco->sens();

    switch (sens)
    {
    case SensHoraire:
    {
        voisin = canton->voisinSP1();
        if (voisin)
        {
            aspectCommande = static_cast<ExccAspect>(voisin->aspectRecu[1]);
            CC_LOG_TRACE("[PilotageDistribue][CC] Sens H → SP1=%u aspect(H)=%u\n",
                         canton->SP1_idx(),
                         static_cast<uint8_t>(aspectCommande));
        }
        else
        {
            voisin = canton->voisinSP2();
            if (voisin)
            {
                aspectCommande = static_cast<ExccAspect>(voisin->aspectRecu[1]);
                CC_LOG_TRACE("[PilotageDistribue][CC] Sens H → SP1 absent → SP2=%u aspect(H)=%u\n",
                             canton->SP2_idx(),
                             static_cast<uint8_t>(aspectCommande));
            }
            else
            {
                CC_LOG_WARN("[PilotageDistribue][CC] Sens H → aucun voisin SP1/SP2\n");
            }
        }
        break;
    }

    case SensAntiHoraire:
    {
        voisin = canton->voisinSM1();
        if (voisin)
        {
            aspectCommande = static_cast<ExccAspect>(voisin->aspectRecu[0]);
            CC_LOG_TRACE("[PilotageDistribue][CC] Sens AH → SM1=%u aspect(AH)=%u\n",
                         canton->SM1_idx(),
                         static_cast<uint8_t>(aspectCommande));
        }
        else
        {
            voisin = canton->voisinSM2();
            if (voisin)
            {
                aspectCommande = static_cast<ExccAspect>(voisin->aspectRecu[0]);
                CC_LOG_TRACE("[PilotageDistribue][CC] Sens AH → SM1 absent → SM2=%u aspect(AH)=%u\n",
                             canton->SM2_idx(),
                             static_cast<uint8_t>(aspectCommande));
            }
            else
            {
                CC_LOG_WARN("[PilotageDistribue][CC] Sens AH → aucun voisin SM1/SM2\n");
            }
        }
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
    case ExccAspect::ASPECT_CARRE:
    case ExccAspect::ASPECT_CARRE_VIOLET:
        CC_LOG_INFO("[PilotageDistribue][CC] Carré (rouge/violet) → arrêt\n");
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
