/*
 * Settings_JSON_Manoeuvre.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Chargement / sauvegarde du MODE MANOEUVRE (voie de service)
 * depuis/vers settings.json.
 *
 * Rôle :
 *   - lire l’état du mode manœuvre dans le JSON
 *   - appliquer cet état au Canton (canton->setModeManoeuvre)
 *   - sauvegarder l’état actuel du canton dans settings.json
 *
 * IMPORTANT :
 *   Ce module ne contient aucune logique métier :
 *     → il synchronise simplement l’état booléen avec le JSON.
 */

#include "Settings.h"
#include "Canton.h"
#include "debug_cc.h"
#include <ArduinoJson.h>

/* ============================================================================
 *  Chargement du mode MANOEUVRE
 * ==========================================================================*/
void Settings_JSON_loadModeManoeuvre(Canton *canton, JsonDocument &doc)
{
    // Valeur par défaut : false (mode normal)
    bool mode = false;

    JsonVariant v = doc["mode_manoeuvre"];
    if (!v.isNull())
        mode = v | false;

    canton->setModeManoeuvre(mode);

    CC_LOG_INFO("[Settings][Manoeuvre][CC] Mode manœuvre chargé : %s\n",
                mode ? "ACTIF" : "INACTIF");
}

/* ============================================================================
 *  Sauvegarde du mode MANOEUVRE
 * ==========================================================================*/
void Settings_JSON_saveModeManoeuvre(Canton *canton, JsonDocument &doc)
{
    bool mode = canton->modeManoeuvre();

    doc["mode_manoeuvre"] = mode;

    CC_LOG_INFO("[Settings][Manoeuvre][CC] Mode manœuvre sauvegardé : %s\n",
                mode ? "ACTIF" : "INACTIF");
}
