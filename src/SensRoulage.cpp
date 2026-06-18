/*
 * SensRoulage.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Déduction du sens de roulage d’une locomotive à partir des capteurs
 * directionnels du Canton Controller (CC).
 *
 * Rôle :
 *   - lire les deux capteurs directionnels (H / AH)
 *   - déterminer le sens de marche de la locomotive présente
 *   - mettre à jour loco->sens() uniquement si l’information est fiable
 *
 * Capteurs :
 *   - Sensor 1 → sens horaire (H)
 *   - Sensor 0 → sens anti‑horaire (AH)
 *
 * Ce module ne modifie pas la vitesse ni l’occupation du canton.
 */

#include "SensRoulage.h"
#include "debug_cc.h"
#include "Canton.h"

// ---------------------------------------------------------------------------
// Déduction du sens de roulage
// ---------------------------------------------------------------------------
void deduireSensRoulage(Canton *canton)
{
    if (!canton)
        return;

    // Lecture des capteurs via l’API moderne
    bool h  = canton->getSensor(1)->state();  // 1 = horaire (H)
    bool ah = canton->getSensor(0)->state();  // 0 = anti‑horaire (AH)

    CC_LOG_TRACE("[SensRoulage][CC] sensorH=%d sensorAH=%d\n", h, ah);

    Loco *loco = canton->getLoco();
    if (!loco)
        return;

    // -----------------------------------------------------------------------
    // Cas 1 : sens horaire détecté
    // -----------------------------------------------------------------------
    if (h && !ah)
    {
        loco->sens(SensHoraire);
        CC_LOG_INFO("[SensRoulage][CC] Sens détecté : HORAIRE\n");
        return;
    }

    // -----------------------------------------------------------------------
    // Cas 2 : sens anti‑horaire détecté
    // -----------------------------------------------------------------------
    if (ah && !h)
    {
        loco->sens(SensAntiHoraire);
        CC_LOG_INFO("[SensRoulage][CC] Sens détecté : ANTI-HORAIRE\n");
        return;
    }

    // -----------------------------------------------------------------------
    // Cas 3 : ambigu ou aucun capteur actif
    // -----------------------------------------------------------------------
    if (h && ah)
    {
        CC_LOG_WARN("[SensRoulage][CC] Ambigu : les deux capteurs actifs → sens non modifié\n");
    }
    else
    {
        CC_LOG_TRACE("[SensRoulage][CC] Aucun capteur actif → sens inchangé\n");
    }
}
