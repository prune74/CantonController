/*
 * PilotageDistribue.cpp — Gestion du pilotage distribué des locomotives
 * Version 2026 — Option A (enum ExsaAspect)
 */

#include "PilotageDistribue.h"
#include "SensEnum.h"
#include "Discovery_Protocol.h"
#include "Node.h"
#include "debug_sa.h"

void executerPilotageDistribue(Node *node)
{
    if (!node)
        return;

    Loco *loco = node->getLoco(); // ← accès moderne
    if (!loco)
        return;

    ExsaAspect aspectCommande = ASPECT_CARRE;
    NodePeriph *voisin = nullptr;

    /*
     * 1) Déterminer le voisin pertinent selon le sens de marche
     */
    SensDeMarche sens = loco->sens();

    switch (sens)
    {
    case SensHoraire:
    {
        uint8_t idx = node->SP1_idx();
        voisin = node->getNodeP(idx);

        if (voisin)
            aspectCommande = static_cast<ExsaAspect>(voisin->aspectRecu[0]);

        SA_LOG_TRACE("[PilotageDistribue] Sens H → SP1=%u aspect=%u\n",
                     idx, aspectCommande);
        break;
    }

    case SensAntiHoraire:
    {
        uint8_t idx = node->SM1_idx();
        voisin = node->getNodeP(idx);

        if (voisin)
            aspectCommande = static_cast<ExsaAspect>(voisin->aspectRecu[1]);

        SA_LOG_TRACE("[PilotageDistribue] Sens AH → SM1=%u aspect=%u\n",
                     idx, aspectCommande);
        break;
    }

    default:
        SA_LOG_WARN("[PilotageDistribue] Sens inconnu → pilotage ignoré\n");
        return;
    }

    /*
     * 2) Pilotage distribué : appliquer la vitesse correspondant à l’aspect
     */
    switch (aspectCommande)
    {
    case ASPECT_CARRE:
        SA_LOG_INFO("[PilotageDistribue] Carré → arrêt\n");
        loco->speed(0);
        break;

    case ASPECT_SEMAPHORE:
    case ASPECT_AVERTISSEMENT:
        SA_LOG_INFO("[PilotageDistribue] Avertissement → %u\n",
                    loco->vitesseAvertissement);
        loco->speed(loco->vitesseAvertissement);
        break;

    case ASPECT_RALENTISSEMENT_30:
        SA_LOG_INFO("[PilotageDistribue] Ralentissement 30 → %u\n",
                    loco->vitesseRalentissement30);
        loco->speed(loco->vitesseRalentissement30);
        break;

    case ASPECT_RALENTISSEMENT_60:
        SA_LOG_INFO("[PilotageDistribue] Ralentissement 60 → %u\n",
                    loco->vitesseRalentissement60);
        loco->speed(loco->vitesseRalentissement60);
        break;

    case ASPECT_RAPPEL_30:
        SA_LOG_INFO("[PilotageDistribue] Rappel 30 → %u\n",
                    loco->vitesseRappel30);
        loco->speed(loco->vitesseRappel30);
        break;

    case ASPECT_RAPPEL_60:
        SA_LOG_INFO("[PilotageDistribue] Rappel 60 → %u\n",
                    loco->vitesseRappel60);
        loco->speed(loco->vitesseRappel60);
        break;

    case ASPECT_VOIE_LIBRE:
        SA_LOG_INFO("[PilotageDistribue] Voie libre → %u\n",
                    loco->vitesseVoieLibre);
        loco->speed(loco->vitesseVoieLibre);
        break;

    case ASPECT_MANOEUVRE:
        SA_LOG_INFO("[PilotageDistribue] Manoeuvre → %u\n",
                    loco->vitesseManoeuvre);
        loco->speed(loco->vitesseManoeuvre);
        break;

    case ASPECT_MASQUE:
        SA_LOG_INFO("[PilotageDistribue] Aspect masqué → %u\n",
                    loco->vitesseAvertissement);
        loco->speed(loco->vitesseAvertissement);
        break;

    default:
        SA_LOG_WARN("[PilotageDistribue] Aspect inconnu (%u) → arrêt sécurité\n",
                    aspectCommande);
        loco->speed(0);
        break;
    }
}
