/*
 * DeductionAspect.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Déduction de l’aspect amont à partir de l’aspect aval (logique BAL complète).
 */

#include "DeductionAspect.h"
#include "Config.h"
#include <Protocol.h>

// ---------------------------------------------------------------------------
// Déduction d’aspect local depuis l’aspect aval
// ---------------------------------------------------------------------------
ExccAspect deduireAspectDepuisAval(ExccAspect aval, bool voieDevie)
{
    switch (aval)
    {
    // 🔴 Carré → avertissement (préparation à l’arrêt)
    case ExccAspect::ASPECT_CARRE:
    case ExccAspect::ASPECT_CARRE_VIOLET:
        return ExccAspect::ASPECT_AVERTISSEMENT;

    // 🔴 Sémaphore → avertissement
    case ExccAspect::ASPECT_SEMAPHORE:
        return ExccAspect::ASPECT_AVERTISSEMENT;

    // 🟡 Avertissement → ralentissement 60 ou 30 selon déviation
    case ExccAspect::ASPECT_AVERTISSEMENT:
        return voieDevie ? ExccAspect::ASPECT_RALENTISSEMENT_30
                         : ExccAspect::ASPECT_RALENTISSEMENT_60;

    // ⏬ Ralentissement 60 → voie libre (droit) ou rappel 60 (dévié)
    case ExccAspect::ASPECT_RALENTISSEMENT_60:
        return voieDevie ? ExccAspect::ASPECT_RAPPEL_60
                         : ExccAspect::ASPECT_VOIE_LIBRE;

    // ⏬ Ralentissement 30 → voie libre (droit) ou rappel 30 (dévié)
    case ExccAspect::ASPECT_RALENTISSEMENT_30:
        return voieDevie ? ExccAspect::ASPECT_RAPPEL_30
                         : ExccAspect::ASPECT_VOIE_LIBRE;

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

    // ⚫ Masqué → avertissement (sécurité)
    case ExccAspect::ASPECT_MASQUE:
        return ExccAspect::ASPECT_AVERTISSEMENT;

    // 🧩 Défaut → carré (sécurité)
    default:
        return ExccAspect::ASPECT_CARRE;
    }
}
