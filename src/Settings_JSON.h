/*
 * Settings_JSON.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Déclarations des modules JSON utilisés par Settings_JSON.cpp.
 *
 * Chaque section du fichier settings.json est gérée par un module dédié :
 *
 *   - Settings_JSON_Generaux.cpp
 *       → paramètres généraux du CC (ID, WiFi, exploration, vitesse, rôle…)
 *
 *   - Settings_JSON_ProfileVoie.cpp
 *       → profil de voie (N / HO / etc.)
 *
 *   - Settings_JSON_Manoeuvre.cpp
 *       → état du mode MANOEUVRE (ON/OFF)
 *
 *   - Settings_JSON_Topologie.cpp
 *       → topologie ferroviaire locale (SP1 / SM1)
 *
 *   - Settings_JSON_Voisins.cpp
 *       → cantons voisins SP2 / SM2 (satellites périphériques)
 *
 *   - Settings_JSON_Aiguilles.cpp
 *       → aiguilles LOGIQUES (indices, positions, liens topologiques)
 *
 *   - Settings_JSON_Signaux.cpp
 *       → signaux SNCF (type de mât + position logique)
 *
 *   - Settings_JSON_Direction.cpp
 *       → configuration directionnelle (feux directionnels, code-barres)
 *
 *   - Settings_JSON_Booster.cpp
 *       → seuils Booster (libre / occupé)
 */

#pragma once

#include <ArduinoJson.h>

class Canton;

/* ============================================================================
   Modules JSON — Chargement (settings.json → objets Canton)
   ==========================================================================*/
void Settings_JSON_loadGeneraux      (Canton *canton, JsonDocument &doc);
void Settings_JSON_loadProfileVoie   (Canton *canton, JsonDocument &doc);
void Settings_JSON_loadModeManoeuvre (Canton *canton, JsonDocument &doc);
void Settings_JSON_loadTopologie     (Canton *canton, JsonDocument &doc);
void Settings_JSON_loadVoisins       (Canton *canton, JsonDocument &doc);
void Settings_JSON_loadAiguilles     (Canton *canton, JsonDocument &doc);
void Settings_JSON_loadSignaux       (Canton *canton, JsonDocument &doc);
void Settings_JSON_loadDirection     (Canton *canton, JsonDocument &doc);
void Settings_JSON_loadBooster       (JsonDocument &doc);

/* ============================================================================
   Modules JSON — Sauvegarde (objets Canton → settings.json)
   ==========================================================================*/
void Settings_JSON_saveGeneraux      (Canton *canton, JsonDocument &doc);
void Settings_JSON_saveProfileVoie   (Canton *canton, JsonDocument &doc);
void Settings_JSON_saveModeManoeuvre (Canton *canton, JsonDocument &doc);
void Settings_JSON_saveTopologie     (Canton *canton, JsonDocument &doc);
void Settings_JSON_saveVoisins       (Canton *canton, JsonDocument &doc);
void Settings_JSON_saveAiguilles     (Canton *canton, JsonDocument &doc);
void Settings_JSON_saveSignaux       (Canton *canton, JsonDocument &doc);
void Settings_JSON_saveDirection     (Canton *canton, JsonDocument &doc);
void Settings_JSON_saveBooster       (JsonDocument &doc);
