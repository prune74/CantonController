/*
 * Settings_JSON_Generaux.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Chargement / sauvegarde des paramètres généraux du Canton :
 *   - ID du canton
 *   - comptAig (Exploration)
 *   - maxSpeed
 *   - sensMarche
 *   - WiFi / Exploration ON/OFF
 *   - SSID / Password
 *
 * IMPORTANT 2026 :
 *   - aucun masque d’aiguilles n’est stocké dans les paramètres généraux
 *   - les masques utiles sont uniquement ceux des CantonPeriph (voisins)
 *   - ce module ne contient aucune logique métier
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
    // ID du canton
    {
        JsonVariant v = doc["idCanton"];
        canton->ID(v.isNull() ? UNUSED_ID : (v | UNUSED_ID));
    }

    // Nombre d’aiguilles détectées (Exploration interne)
    {
        JsonVariant v = doc["comptAig"];
        Exploration::comptAig(v.isNull() ? 0 : (v | 0));
    }

    // Vitesse max
    {
        JsonVariant v = doc["maxSpeed"];
        canton->maxSpeed(v.isNull() ? 128 : (v | 128));
    }

    // Sens de marche
    {
        JsonVariant v = doc["sensMarche"];
        canton->sensMarche(static_cast<SensDeMarche>(v.isNull() ? 0 : (v | 0)));
    }

    // WiFi / Exploration
    {
        JsonVariant v = doc["wifi_on"];
        Settings::WIFI_ON = v.isNull() ? true : (v | true);
    }
    {
        JsonVariant v = doc["exploration_on"];
        Settings::EXPLORATION_ON = v.isNull() ? true : (v | true);
    }

    // Mode autonome
    {
        JsonVariant v = doc["standalone"];
        Settings::STANDALONE = v.isNull() ? false : (v | false);
    }

    // SSID / Password
    {
        JsonVariant v = doc["ssid"];
        Settings::ssid_str = v.isNull() ? "" : String(v | "");
    }
    {
        JsonVariant v = doc["password"];
        Settings::password_str = v.isNull() ? "" : String(v | "");
    }

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

    doc["maxSpeed"]       = canton->maxSpeed();
    doc["sensMarche"]     = (uint8_t)canton->sensMarche();

    doc["wifi_on"]        = Settings::WIFI_ON;
    doc["exploration_on"] = Settings::EXPLORATION_ON;

    doc["ssid"]           = Settings::ssid;
    doc["password"]       = Settings::password;

    doc["standalone"]     = Settings::STANDALONE;
}
