/*
 * Settings_JSON_ProfileVoie.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Chargement / sauvegarde du PROFIL DE VOIE depuis/vers settings.json.
 *
 * Rôle :
 *   - charger le profil de voie (N / HO / etc.)
 *   - sauvegarder le profil de voie
 *
 * Ce module ne contient aucune logique métier :
 *   → il synchronise simplement l’état logique avec le JSON.
 */

#include "Settings.h"
#include "Canton.h"
#include "debug_cc.h"
#include <ArduinoJson.h>

/* ============================================================================
 *  Chargement du profil de voie
 * ==========================================================================*/
void Settings_JSON_loadProfileVoie(Canton *canton, JsonDocument &doc)
{
    JsonVariant v = doc["track_profile"];

    if (!v.isNull())
        Settings::TRACK_PROFILE = v | false;
    else
        Settings::TRACK_PROFILE = false;

    CC_LOG_INFO("[Settings][ProfileVoie][CC] Profil de voie chargé : %s\n",
                Settings::TRACK_PROFILE ? "15V (HO)" : "12V (N)");
}

/* ============================================================================
 *  Sauvegarde du profil de voie
 * ==========================================================================*/
void Settings_JSON_saveProfileVoie(Canton *canton, JsonDocument &doc)
{
    doc["track_profile"] = Settings::TRACK_PROFILE;

    CC_LOG_INFO("[Settings][ProfileVoie][CC] Profil de voie sauvegardé : %s\n",
                Settings::TRACK_PROFILE ? "15V (HO)" : "12V (N)");
}
