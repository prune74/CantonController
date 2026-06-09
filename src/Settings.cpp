/*
 * Settings.cpp — Orchestrateur principal (Exploration 2026)
 * ---------------------------------------------------------------------------
 * Rôle :
 *   - Centraliser l’accès aux paramètres du SA
 *   - Orchestrer l’initialisation complète :
 *        1) UART RS485 (EXSA)
 *        2) SPIFFS
 *        3) settings.json
 *        4) Dialogue CAN avec la carte Main
 *
 * Ce fichier NE CONTIENT PAS la logique détaillée :
 *   - UART  → Settings_UART.cpp
 *   - SPIFFS → Settings_SPIFFS.cpp
 *   - JSON → Settings_JSON.cpp
 *   - CAN → Settings_CAN.cpp
 */

#include "Settings.h"
#include "debug_sa.h"
#include "SA_UartRx.h"
#include "Config.h"
#include "Canton.h"

/* ============================================================================
 *  Déclarations statiques
 * ==========================================================================*/

bool Settings::WIFI_ON = true;
bool Settings::EXPLORATION_ON = true;
String Settings::ssid_str = "";
String Settings::password_str = "";
char Settings::ssid[64] = {};
char Settings::password[64] = {};
bool Settings::isMainReady = false;
Canton *Settings::canton = nullptr;

/* ============================================================================
 *  Paramètres globaux : WiFi / Exploration
 * ==========================================================================*/

bool Settings::wifiOn()
{
    return Settings::WIFI_ON;
}

void Settings::wifiOn(bool val)
{
    Settings::WIFI_ON = val;
    SA_LOG_INFO("[Settings] wifiOn = %d\n", val);
}

bool Settings::explorationOn()
{
    return Settings::EXPLORATION_ON;
}

void Settings::explorationOn(bool val)
{
    Settings::EXPLORATION_ON = val;
    SA_LOG_INFO("[Settings] explorationOn = %d\n", val);
}

/* ============================================================================
 *  setup() — Initialisation complète SA (hors CAN)
 * ---------------------------------------------------------------------------
 * Étapes :
 *   1) Initialisation UART RS485 (communication EXSA)
 *   2) Montage SPIFFS (système de fichiers interne)
 *   3) Lecture du fichier settings.json
 *
 * Remarque :
 *   Le dialogue CAN avec la carte Main est effectué dans begin().
 * ==========================================================================*/
void Settings::setup(Canton *nd)
{
    canton = nd;

    SA_LOG_INFO("[Settings] Initialisation complète du SA...\n");

    // ------------------------------------------------------------------------
    // 1) UART RS485 (EXSA)
    // ------------------------------------------------------------------------

    SA_LOG_TRACE("[Settings] Initialisation UART RS485...\n");
    setupUART();

    // ------------------------------------------------------------------------
    // 1) UART RS485 (EXSA) - Lancement de la tâche de réception
    // ------------------------------------------------------------------------

    SA_LOG_TRACE("[Settings] Lancement de la tâche de réception UART...\n");
    SA_UartRx::begin();

    // ------------------------------------------------------------------------
    // 2) Montage SPIFFS
    // ------------------------------------------------------------------------
    SA_LOG_TRACE("[Settings] Montage SPIFFS...\n");

    if (!mountSPIFFS())
    {
        SA_LOG_ERROR("[Settings] SPIFFS indisponible → arrêt de setup()\n");
        return;
    }

    // ------------------------------------------------------------------------
    // 3) Lecture settings.json
    // ------------------------------------------------------------------------
    SA_LOG_TRACE("[Settings] Lecture settings.json...\n");
    Settings::load(); // ← charge ssid/password dans settingsDoc et dans les champs statiques
    loadFile(canton); // ← NOUVELLE API 2026

    SA_LOG_INFO("[Settings] ✔ setup() terminé\n\n");
}

/* ============================================================================
 *  begin() — Dialogue CAN avec la carte Main
 * ---------------------------------------------------------------------------
 * Cette fonction :
 *   - établit la communication CAN avec la carte Main
 *   - vérifie que la carte Main est prête
 *   - synchronise les paramètres initiaux
 *
 * L’implémentation réelle est dans Settings_CAN.cpp.
 * ==========================================================================*/
bool Settings::begin()
{
    SA_LOG_INFO("[Settings] Début du dialogue CAN avec la carte Main...\n");

    bool ok = Settings::beginCAN();

    if (ok)
        SA_LOG_INFO("[Settings] ✔ Initialisation CAN réussie\n\n");
    else
        SA_LOG_ERROR("[Settings] ❌ Erreur CAN\n\n");

    return ok;
}
