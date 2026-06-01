/*
   WebHandler_Settings.cpp — Discovery 2026 (FINAL & CLEAN)
*/

#include "WebHandler.h"
#include "debug_sa.h"
#include "Settings.h"
#include "Discovery.h"

#include <SPIFFS.h>
#include <FS.h>
#include <ArduinoJson.h>

// ---------------------------------------------------------------------------
// handleWifi()
// ---------------------------------------------------------------------------
void WebHandler::handleWifi(JsonDocument &doc)
{
    bool wifi_on = doc["wifi_on"];

    SA_LOG_INFO("[Settings] wifi_on = %d\n", wifi_on);

    Settings::wifiOn(wifi_on);
    Settings::save();

    // Si tu veux un reboot automatique :
    // ESP.restart();
}

// ---------------------------------------------------------------------------
// handleDiscovery()
// ---------------------------------------------------------------------------
void WebHandler::handleDiscovery(JsonDocument &doc)
{
    bool discovery_on = doc["discovery_on"];

    SA_LOG_INFO("[Settings] discovery_on = %d\n", discovery_on);

    Settings::discoveryOn(discovery_on);
    Settings::save();

    if (!discovery_on)
    {
        SA_LOG_INFO("[Settings] Arrêt du processus Discovery\n");
        Discovery::stopProcess(true);
    }
}

// ---------------------------------------------------------------------------
// handleSave()
// ---------------------------------------------------------------------------
// Sauvegarde complète de settings.json, y compris servoCfg[] et Booster
// ---------------------------------------------------------------------------
void WebHandler::handleSave()
{
    SA_LOG_INFO("[Settings] Sauvegarde settings.json (servos + booster + params)\n");

    if (!SPIFFS.begin(true))
    {
        SA_LOG_ERROR("[Settings] SPIFFS indisponible, impossible de sauvegarder\n");
        return;
    }

    StaticJsonDocument<4096> doc;

    // Charger l’existant
    File file = SPIFFS.open("/settings.json", "r");
    if (file)
    {
        DeserializationError err = deserializeJson(doc, file);
        file.close();

        if (err)
            SA_LOG_WARN("[Settings] Erreur JSON existant : %s\n", err.c_str());
    }

    // ------------------------------------------------------------
    // Sauvegarde des servos (slider 0–10)
    // ------------------------------------------------------------
    for (uint8_t i = 0; i < 6; ++i)
    {
        char keyPosDroit[16];
        char keyPosDevie[16];
        char keySpeed[16];

        snprintf(keyPosDroit, sizeof(keyPosDroit), "aig%uposDroit", i);
        snprintf(keyPosDevie, sizeof(keyPosDevie), "aig%uposDevie", i);
        snprintf(keySpeed,    sizeof(keySpeed),    "aig%uspeed",    i);

        doc[keyPosDroit] = servoCfg[i].posDroit;
        doc[keyPosDevie] = servoCfg[i].posDevie;

        // On stocke le slider 0–10, pas la vitesse µs/s
        doc[keySpeed]    = servoCfg[i].speed;
    }

    // ------------------------------------------------------------
    // Sauvegarde Booster (seuils)
    // ------------------------------------------------------------
    doc["booster_seuil_libre"]  = Settings::boosterSeuilLibre();
    doc["booster_seuil_occupe"] = Settings::boosterSeuilOccupe();

    // ------------------------------------------------------------
    // Écriture du JSON mis à jour
    // ------------------------------------------------------------
    file = SPIFFS.open("/settings.json", "w");
    if (!file)
    {
        SA_LOG_ERROR("[Settings] Impossible d’ouvrir settings.json en écriture\n");
        return;
    }

    serializeJsonPretty(doc, file);
    file.close();

    SA_LOG_INFO("[Settings] settings.json sauvegardé avec succès\n");
}

// ---------------------------------------------------------------------------
// handleRestart()
// ---------------------------------------------------------------------------
void WebHandler::handleRestart()
{
    SA_LOG_WARN("[Settings] Redémarrage demandé\n");
    ESP.restart();
}
