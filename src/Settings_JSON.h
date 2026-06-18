/*
 * Settings_JSON.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Déclarations des modules JSON utilisés par Settings_JSON.cpp.
 *
 * Chaque section du fichier settings.json est gérée par un module dédié :
 *   - Settings_JSON_Generaux.cpp   → paramètres généraux du CC
 *   - Settings_JSON_Topologie.cpp  → topologie ferroviaire (SP1/SM1)
 *   - Settings_JSON_Voisins.cpp    → cantons voisins (SP2/SM2)
 *   - Settings_JSON_Aiguilles.cpp  → aiguilles locales
 *   - Settings_JSON_Signaux.cpp    → signaux SNCF (profils + positions)
 *   - Settings_JSON_Direction.cpp  → sens de marche / rôle ferroviaire
 *   - Settings_JSON_Booster.cpp    → seuils Booster
 *
 * Ce header centralise toutes les signatures de chargement et sauvegarde.
 */

#pragma once

#include <ArduinoJson.h>

class Canton;

/* ============================================================================
   Modules JSON — Fonctions de chargement (settings.json → CC)
   ==========================================================================*/
void Settings_JSON_loadGeneraux   (Canton *canton, JsonDocument &doc);
void Settings_JSON_loadTopologie  (Canton *canton, JsonDocument &doc);
void Settings_JSON_loadVoisins    (Canton *canton, JsonDocument &doc);
void Settings_JSON_loadAiguilles  (Canton *canton, JsonDocument &doc);
void Settings_JSON_loadSignaux    (Canton *canton, JsonDocument &doc);
void Settings_JSON_loadDirection  (Canton *canton, JsonDocument &doc);
void Settings_JSON_loadBooster    (JsonDocument &doc);

/* ============================================================================
   Modules JSON — Fonctions de sauvegarde (CC → settings.json)
   ==========================================================================*/
void Settings_JSON_saveGeneraux   (Canton *canton, JsonDocument &doc);
void Settings_JSON_saveTopologie  (Canton *canton, JsonDocument &doc);
void Settings_JSON_saveVoisins    (Canton *canton, JsonDocument &doc);
void Settings_JSON_saveAiguilles  (Canton *canton, JsonDocument &doc);
void Settings_JSON_saveSignaux    (Canton *canton, JsonDocument &doc);
void Settings_JSON_saveDirection  (Canton *canton, JsonDocument &doc);
void Settings_JSON_saveBooster    (JsonDocument &doc);
