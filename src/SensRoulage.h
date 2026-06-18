/*
 * SensRoulage.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Déduction du sens de roulage d’une locomotive à partir des capteurs
 * directionnels du Canton Controller (CC).
 *
 * Capteurs utilisés :
 *   - Sensor 1 → sens horaire (H)
 *   - Sensor 0 → sens anti‑horaire (AH)
 *
 * Ce module met à jour loco->sens() uniquement lorsque l’information
 * est fiable. Il ne modifie ni la vitesse ni l’occupation du canton.
 */

#pragma once

#include "Canton.h"
#include "SensEnum.h"

// Déduit le sens de roulage à partir des capteurs H / AH
void deduireSensRoulage(Canton *canton);
