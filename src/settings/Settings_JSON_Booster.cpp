/*
 * Settings_JSON_Booster.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Gestion des seuils du Booster dans settings.json.
 *
 * Clés JSON :
 *   - "booster_seuil_libre"
 *   - "booster_seuil_occupe"
 *
 * Ce module ne contient aucune logique métier :
 *   → il synchronise simplement les seuils entre JSON et Settings.
 */

#include "Settings.h"
#include "debug_cc.h"
#include <ArduinoJson.h>

/* ============================================================================
 *  Chargement des seuils Booster
 * ==========================================================================*/
void Settings_JSON_loadBooster(JsonDocument &doc)
{
    Settings::setBoosterSeuilLibre(doc["booster_seuil_libre"] | 0);
    Settings::setBoosterSeuilOccupe(doc["booster_seuil_occupe"] | 0);

    CC_LOG_INFO("[Settings][Booster][CC] Seuils chargés → libre=%u occupe=%u\n",
                Settings::boosterSeuilLibre(),
                Settings::boosterSeuilOccupe());
}

/* ============================================================================
 *  Sauvegarde des seuils Booster
 * ==========================================================================*/
void Settings_JSON_saveBooster(JsonDocument &doc)
{
    doc["booster_seuil_libre"]  = Settings::boosterSeuilLibre();
    doc["booster_seuil_occupe"] = Settings::boosterSeuilOccupe();
}
