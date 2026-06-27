/*
 * DeductionAspect.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Déduction de l’aspect amont à partir de l’aspect aval.
 */

#include "DeductionAspect.h"
#include "Config.h"
#include "Protocol.h"

// ---------------------------------------------------------------------------
// Déduction d’aspect local depuis l’aspect aval
// ---------------------------------------------------------------------------
ExccAspect deduireAspectDepuisAval(ExccAspect aval, bool voieDevie)
{
    switch (aval)
    {
    // 🔴 Carré → carré
    case ExccAspect::ASPECT_CARRE:
        return ExccAspect::ASPECT_CARRE;

    // 🟣 Carré Violet → carré Violet
    case ExccAspect::ASPECT_CARRE_VIOLET:
        return ExccAspect::ASPECT_CARRE_VIOLET;

    // 🔴 Sémaphore → avertissement
    case ExccAspect::ASPECT_SEMAPHORE:
        return ExccAspect::ASPECT_AVERTISSEMENT;

    // 🟡 Avertissement → voie libre
    case ExccAspect::ASPECT_AVERTISSEMENT:
        return ExccAspect::ASPECT_VOIE_LIBRE;

    // ⏬ Ralentissement 30 → rappel 30 si voie déviée
    case ExccAspect::ASPECT_RALENTISSEMENT_30:
        return voieDevie ? ExccAspect::ASPECT_RAPPEL_30
                         : ExccAspect::ASPECT_RALENTISSEMENT_30;

    // ⏬ Ralentissement 60 → rappel 60 si voie déviée
    case ExccAspect::ASPECT_RALENTISSEMENT_60:
        return voieDevie ? ExccAspect::ASPECT_RAPPEL_60
                         : ExccAspect::ASPECT_RALENTISSEMENT_60;

    // ⏸ Rappel 30 → rappel 30
    case ExccAspect::ASPECT_RAPPEL_30:
        return ExccAspect::ASPECT_RAPPEL_30;

    // ⏸ Rappel 60 → rappel 60
    case ExccAspect::ASPECT_RAPPEL_60:
        return ExccAspect::ASPECT_RAPPEL_60;

    // 🟢 Voie libre → voie libre
    case ExccAspect::ASPECT_VOIE_LIBRE:
        return ExccAspect::ASPECT_VOIE_LIBRE;

    // 🔵 Manoeuvre → manoeuvre
    case ExccAspect::ASPECT_MANOEUVRE:
        return ExccAspect::ASPECT_MANOEUVRE;

    // ⚫ Masqué → avertissement
    case ExccAspect::ASPECT_MASQUE:
        return ExccAspect::ASPECT_AVERTISSEMENT;

    // 🧩 Défaut → carré
    default:
        return ExccAspect::ASPECT_CARRE;
    }
}
