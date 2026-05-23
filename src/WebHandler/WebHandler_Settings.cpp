/*
   WebHandler_Settings.cpp — Version corrigée Discovery 2026
   ------------------------------------------------------------
   - Sauvegarde settings.json correctement
   - Met à jour les valeurs servo dans le JSON
   - Ne touche plus à Settings::node (Node*)
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
    bool wifi_on = doc["wifi_on"][0];

    SA_LOG_WARN("[Settings] wifi_on = %d → sauvegarde + reboot\n", wifi_on);

    Settings::wifiOn(wifi_on);

    // Sauvegarde du JSON actuel
    Settings::writeFile(Settings::node);

    delay(1000);
    ESP.restart();
}

// ---------------------------------------------------------------------------
// handleDiscovery()
// ---------------------------------------------------------------------------
void WebHandler::handleDiscovery(JsonDocument &doc)
{
    bool discovery_on = doc["discovery_on"][0];

    SA_LOG_INFO("[Settings] discovery_on = %d\n", discovery_on);

    Settings::discoveryOn(discovery_on);

    if (!discovery_on)
    {
        SA_LOG_INFO("[Settings] Arrêt du processus Discovery\n");
        Discovery::stopProcess(true);
    }

    Settings::writeFile(Settings::node);
}

// ---------------------------------------------------------------------------
// handleSave()
// ---------------------------------------------------------------------------
// Sauvegarde complète de settings.json, y compris servoCfg[]
// ---------------------------------------------------------------------------
void WebHandler::handleSave()
{
    SA_LOG_INFO("[Settings] Sauvegarde settings.json (servos + paramètres)\n");

    if (!SPIFFS.begin(true))
    {
        SA_LOG_ERROR("[Settings] SPIFFS indisponible, impossible de sauvegarder\n");
        return;
    }

    // Charger le JSON existant
    StaticJsonDocument<2048> doc;

    File file = SPIFFS.open("/settings.json", "r");
    if (file)
    {
        DeserializationError error = deserializeJson(doc, file);
        file.close();

        if (error)
        {
            SA_LOG_ERROR("[Settings] Erreur JSON à la lecture : %s\n", error.c_str());
        }
    }
    else
    {
        SA_LOG_WARN("[Settings] settings.json introuvable → création d’un nouveau document\n");
    }

    // ------------------------------------------------------------
    // Mise à jour des réglages servo dans le JSON
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
        doc[keySpeed]    = servoCfg[i].speed;
    }

    // ------------------------------------------------------------
    // Écriture du JSON mis à jour
    // ------------------------------------------------------------
    file = SPIFFS.open("/settings.json", "w");
    if (!file)
    {
        SA_LOG_ERROR("[Settings] Impossible d’ouvrir settings.json en écriture\n");
        return;
    }

    if (serializeJson(doc, file) == 0)
    {
        SA_LOG_ERROR("[Settings] Erreur lors de l’écriture de settings.json\n");
    }
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
