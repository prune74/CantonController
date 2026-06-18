/*
 * Settings_JSON_Generaux.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Chargement / sauvegarde des paramètres généraux du Canton :
 *   - ID du canton
 *   - comptAig (Exploration)
 *   - masqueAig
 *   - maxSpeed
 *   - sensMarche
 *   - WiFi / Exploration ON/OFF
 *   - SSID / Password
 *
 * Ce module ne contient aucune logique métier :
 *   → il synchronise simplement les données JSON ↔ structures Canton / Settings.
 */

#include "Settings.h"
#include "Canton.h"
#include "Exploration.h"
#include "debug_cc.h"
#include <ArduinoJson.h>

/* ============================================================================
 *  Chargement des paramètres généraux
 * ==========================================================================*/
void Settings_JSON_loadGeneraux(Canton *canton, JsonDocument &doc)
{
    // -----------------------------------------------------------------------
    // ID du canton
    // -----------------------------------------------------------------------
    canton->ID(doc["idCanton"] | UNUSED_ID);

    // -----------------------------------------------------------------------
    // Nombre d’aiguilles détectées (Exploration interne)
    // -----------------------------------------------------------------------
    Exploration::comptAig(doc["comptAig"] | 0);

    // -----------------------------------------------------------------------
    // Masque aiguilles principal
    // -----------------------------------------------------------------------
    canton->masqueAig(doc["masqueAig"] | 0);

    // -----------------------------------------------------------------------
    // Vitesse max
    // -----------------------------------------------------------------------
    canton->maxSpeed(doc["maxSpeed"] | 128);

    // -----------------------------------------------------------------------
    // Sens de marche
    // -----------------------------------------------------------------------
    canton->sensMarche(static_cast<SensDeMarche>(doc["sensMarche"] | 0));

    // -----------------------------------------------------------------------
    // WiFi / Exploration
    // -----------------------------------------------------------------------
    Settings::WIFI_ON        = doc["wifi_on"]        | true;
    Settings::EXPLORATION_ON = doc["exploration_on"] | true;

    // -----------------------------------------------------------------------
    // SSID / Password
    // -----------------------------------------------------------------------
    Settings::ssid_str     = String((const char *)(doc["ssid"]     | ""));
    Settings::password_str = String((const char *)(doc["password"] | ""));

    strncpy(Settings::ssid,     Settings::ssid_str.c_str(),     sizeof(Settings::ssid) - 1);
    strncpy(Settings::password, Settings::password_str.c_str(), sizeof(Settings::password) - 1);

    CC_LOG_INFO("[Settings][Generaux][CC] Paramètres généraux chargés\n");
}

/* ============================================================================
 *  Sauvegarde des paramètres généraux
 * ==========================================================================*/
void Settings_JSON_saveGeneraux(Canton *canton, JsonDocument &doc)
{
    doc["idCanton"]       = canton->ID();
    doc["comptAig"]       = Exploration::comptAig();
    doc["masqueAig"]      = canton->masqueAig();

    doc["maxSpeed"]       = canton->maxSpeed();
    doc["sensMarche"]     = (uint8_t)canton->sensMarche();

    doc["wifi_on"]        = Settings::WIFI_ON;
    doc["exploration_on"] = Settings::EXPLORATION_ON;

    doc["ssid"]           = Settings::ssid;
    doc["password"]       = Settings::password;
}
