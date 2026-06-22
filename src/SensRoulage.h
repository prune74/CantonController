/*
 * SensRoulage.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Déduction du sens de roulage d’une locomotive à partir des capteurs
 * directionnels envoyés par l’EXCC.
 *
 * Capteurs utilisés :
 *   - ponctuelH  → sens horaire (H)
 *   - ponctuelAH → sens anti‑horaire (AH)
 *
 * Le CC ne lit plus les capteurs physiques :
 *   → EXCC lit les capteurs
 *   → EXCC envoie l’état logique (0 = libre, 1 = actif)
 *   → Le CC stocke uniquement l’état logique dans le Canton
 *
 * Ce module met à jour loco->sens() uniquement lorsque l’information
 * est fiable. Il ne modifie ni la vitesse ni l’occupation du canton.
 */

#pragma once

#include "Canton.h"
#include "SensEnum.h"

// Déduit le sens de roulage à partir des états ponctuels H / AH
void deduireSensRoulage(Canton *canton);
