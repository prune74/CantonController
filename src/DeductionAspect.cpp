/*
DeductionAspect.cpp - Gestion de la déduction d’aspect local depuis l’aspect aval
*/

#include "DeductionAspect.h"
#include "Config.h"               // pour USE_CLIGNOTANTS
#include "Exploration_Protocol.h" // pour ExsaAspect

/*************************************************************************************
 * Fonction de déduction d’aspect local depuis l’aspect aval
 * La logique respecte les règles SNCF :
 * - Si l’aval est restrictif, l’amont doit l’être aussi
 * - Si la voie est déviée, on limite l’ouverture
 * - Les aspects clignotants peuvent être conservés ou simplifiés selon USE_CLIGNOTANTS
 *************************************************************************************/
ExsaAspect deduireAspectDepuisAval(ExsaAspect aval, bool voieDevie)
{
    switch (aval)
    {
    // 🔴 Carré → carré
    case ASPECT_CARRE:
        return ASPECT_CARRE;

    // 🟠 Sémaphore → avertissement
    case ASPECT_SEMAPHORE:
        return ASPECT_AVERTISSEMENT;

    // 🟡 Avertissement → voie libre
    case ASPECT_AVERTISSEMENT:
        return ASPECT_VOIE_LIBRE;

    // ⏬ Ralentissement 30 → rappel 30 si voie déviée
    case ASPECT_RALENTISSEMENT_30:
        return voieDevie ? ASPECT_RAPPEL_30 : ASPECT_RALENTISSEMENT_30;

    // ⏬ Ralentissement 60 → rappel 60 si voie déviée
    case ASPECT_RALENTISSEMENT_60:
        return voieDevie ? ASPECT_RAPPEL_60 : ASPECT_RALENTISSEMENT_60;

    // ⏸ Rappel 30 → rappel 30
    case ASPECT_RAPPEL_30:
        return ASPECT_RAPPEL_30;

    // ⏸ Rappel 60 → rappel 60
    case ASPECT_RAPPEL_60:
        return ASPECT_RAPPEL_60;

    // 🟢 Voie libre → voie libre
    case ASPECT_VOIE_LIBRE:
        return ASPECT_VOIE_LIBRE;

    // ⚪ Manœuvre → manœuvre
    case ASPECT_MANOEUVRE:
        return ASPECT_MANOEUVRE;

    // ⚫ Masqué → avertissement
    case ASPECT_MASQUE:
        return ASPECT_AVERTISSEMENT;

    // 🧩 Défaut → carré
    default:
        return ASPECT_CARRE;
    }
}
