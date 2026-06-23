/*
 * SensRoulage.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Déduction du sens de roulage d’une locomotive à partir des capteurs
 * directionnels envoyés par l’EXCC.
 *
 * Rôle :
 *   - lire les états ponctuels H / AH du canton
 *   - déterminer le sens de marche de la locomotive
 *   - mettre à jour loco->sens() uniquement si l’information est fiable
 *
 * IMPORTANT :
 *   - les capteurs physiques ne sont plus lus par le CC
 *   - EXCC envoie l’état logique (0 = libre, 1 = actif)
 */

#include "SensRoulage.h"
#include "debug_cc.h"
#include "Canton.h"

// ---------------------------------------------------------------------------
// Déduction du sens de roulage
// ---------------------------------------------------------------------------
void deduireSensRoulage(Canton *canton) // 🟢
{
    if (!canton)
        return;

    // Lecture des états ponctuels envoyés par EXCC
    bool h  = canton->ponctuelH();   // côté horaire
    bool ah = canton->ponctuelAH();  // côté anti‑horaire

    CC_LOG_TRACE("[SensRoulage][CC] ponctuelH=%d ponctuelAH=%d\n", h, ah);

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
