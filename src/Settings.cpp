/*
 * Settings.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Orchestrateur principal du Canton Controller (CC).
 *
 * Rôle :
 *   - Initialisation générale du CC
 *   - Montage SPIFFS (module dédié)
 *   - Lecture du fichier settings.json (format JSON 2026)
 *   - Chargement des paramètres : généraux, topologie, voisins,
 *     aiguilles, signaux, direction, booster
 *   - Lancement du dialogue CAN avec la carte Main (EXCC)
 *
 * Ce module centralise l’accès aux paramètres persistants du CC.
 */

#include "Settings.h"
#include "Settings_JSON.h"
#include "debug_cc.h"
#include "CC_UartRx.h"
#include "Config.h"
#include "Canton.h"
#include <SPIFFS.h>
#include <ArduinoJson.h>

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

// Booster
uint16_t Settings::s_boosterSeuilLibre  = 0;
uint16_t Settings::s_boosterSeuilOccupe = 0;

/* ============================================================================
 *  Paramètres globaux : WiFi / Exploration
 * ==========================================================================*/

bool Settings::wifiOn() { return WIFI_ON; }
void Settings::wifiOn(bool val) { WIFI_ON = val; }

bool Settings::explorationOn() { return EXPLORATION_ON; }
void Settings::explorationOn(bool val) { EXPLORATION_ON = val; }

/* ============================================================================
 *  setup() — Initialisation complète du Canton Controller (hors CAN)
 * ==========================================================================*/

void Settings::setup(Canton *nd)
{
    canton = nd;

    CC_LOG_INFO("[Settings][CC] Initialisation complète du CC...\n");

    // ------------------------------------------------------------------------
    // UART (module séparé)
    // ------------------------------------------------------------------------
    setupUART();
    CC_UartRx::begin();

    // ------------------------------------------------------------------------
    // SPIFFS (module séparé)
    // ------------------------------------------------------------------------
    if (!mountSPIFFS())
        return;

    // ------------------------------------------------------------------------
    // Lecture du fichier JSON 2026
    // ------------------------------------------------------------------------
    loadFile(canton);

    CC_LOG_INFO("[Settings][CC] ✔ setup() terminé\n\n");
}

/* ============================================================================
 *  begin() — Dialogue CAN avec la carte Main (EXCC)
 * ==========================================================================*/

bool Settings::begin()
{
    CC_LOG_INFO("[Settings][CC] Début du dialogue CAN...\n");

    bool ok = beginCAN();

    if (ok)
        CC_LOG_INFO("[Settings][CC] ✔ Initialisation CAN réussie\n");
    else
        CC_LOG_ERROR("[Settings][CC] ❌ Erreur CAN\n");

    return ok;
}

/* ============================================================================
 *  JSON 2026 — loadFile()
 * ==========================================================================*/

void Settings::loadFile(Canton *canton)
{
    File file = SPIFFS.open("/settings.json", "r");
    if (!file)
    {
        CC_LOG_WARN("[Settings][CC] settings.json introuvable\n");
        return;
    }

    StaticJsonDocument<8192> doc;
    DeserializationError err = deserializeJson(doc, file);
    file.close();

    if (err)
    {
        CC_LOG_ERROR("[Settings][CC] Erreur JSON: %s\n", err.c_str());
        return;
    }

    // Chargement des sections JSON 2026
    Settings_JSON_loadGeneraux(canton, doc);
    Settings_JSON_loadTopologie(canton, doc);
    Settings_JSON_loadVoisins(canton, doc);
    Settings_JSON_loadAiguilles(canton, doc);
    Settings_JSON_loadSignaux(canton, doc);
    Settings_JSON_loadDirection(canton, doc);
    Settings_JSON_loadBooster(doc);

    // Calcul du rôle ferroviaire (entrée / sortie / neutre)
    canton->computeRole();

    CC_LOG_INFO("[Settings][CC] loadFile() terminé\n");
}

/* ============================================================================
 *  JSON 2026 — writeFile()
 * ==========================================================================*/

void Settings::writeFile(Canton *canton)
{
    StaticJsonDocument<8192> doc;

    // Sauvegarde des sections JSON 2026
    Settings_JSON_saveGeneraux(canton, doc);
    Settings_JSON_saveTopologie(canton, doc);
    Settings_JSON_saveVoisins(canton, doc);
    Settings_JSON_saveAiguilles(canton, doc);
    Settings_JSON_saveSignaux(canton, doc);
    Settings_JSON_saveDirection(canton, doc);
    Settings_JSON_saveBooster(doc);

    File file = SPIFFS.open("/settings.json", "w");
    if (!file)
    {
        CC_LOG_ERROR("[Settings][CC] Impossible d’écrire settings.json\n");
        return;
    }

    serializeJsonPretty(doc, file);
    file.close();

    CC_LOG_INFO("[Settings][CC] writeFile() terminé\n");
}
