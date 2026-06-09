/*
 * SensRoulage.cpp — Déduction du sens de roulage (Exploration 2026)
 */

#include "SensRoulage.h"
#include "debug_sa.h"
#include "Canton.h"

/*************************************************************************************
 * Déduction du sens de roulage
 ************************************************************************************/
void deduireSensRoulage(Canton *canton)
{
    if (!canton)
        return;

    // Lecture des capteurs via l’API moderne
    bool h = canton->getSensor(1)->state();  // 1 = horaire (H)
    bool ah = canton->getSensor(0)->state(); // 0 = anti-horaire (AH)

    SA_LOG_TRACE("[SensRoulage] sensorH=%d sensorAH=%d\n", h, ah);

    Loco *loco = canton->getLoco();
    if (!loco)
        return;

    // Cas 1 : sens horaire détecté
    if (h && !ah)
    {
        loco->sens(SensHoraire);
        SA_LOG_INFO("[SensRoulage] Sens détecté : HORAIRE\n");
        return;
    }

    // Cas 2 : sens anti-horaire détecté
    if (ah && !h)
    {
        loco->sens(SensAntiHoraire);
        SA_LOG_INFO("[SensRoulage] Sens détecté : ANTI-HORAIRE\n");
        return;
    }

    // Cas 3 : ambigu ou aucun capteur actif
    if (h && ah)
    {
        SA_LOG_WARN("[SensRoulage] Ambigu : les deux capteurs actifs → sens non modifié\n");
    }
    else
    {
        SA_LOG_TRACE("[SensRoulage] Aucun capteur actif → sens inchangé\n");
    }
}
