/*
 * WebHandler_Settings.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Gestion des paramètres système reçus via WebSocket :
 *   - activation WiFi
 *   - activation Exploration
 *   - sauvegarde settings.json
 *   - redémarrage ESP32
 *
 * Ce module ne contient aucune logique métier :
 *   → il met à jour Settings et déclenche les actions système associées.
 */

#include "WebHandler.h"
#include "debug_cc.h"
#include "Settings.h"
#include "Exploration.h"

// ---------------------------------------------------------------------------
// handleWifi()
// ---------------------------------------------------------------------------
void WebHandler::handleWifi(JsonDocument &doc)
{
    bool wifi_on = doc["wifi_on"];

    CC_LOG_INFO("[Settings][CC] wifi_on = %d\n", wifi_on);

    Settings::wifiOn(wifi_on);

    // Sauvegarde JSON 2026
    Settings::writeFile(canton);
}

// ---------------------------------------------------------------------------
// handleExploration()
// ---------------------------------------------------------------------------
void WebHandler::handleExploration(JsonDocument &doc)
{
    bool exploration_on = doc["exploration_on"];

    CC_LOG_INFO("[Settings][CC] exploration_on = %d\n", exploration_on);

    Settings::explorationOn(exploration_on);

    // Sauvegarde JSON 2026
    Settings::writeFile(canton);

    if (!exploration_on)
    {
        CC_LOG_INFO("[Settings][CC] Arrêt du processus Exploration\n");
        Exploration::stopProcess(true);
    }
}

// ---------------------------------------------------------------------------
// handleSave()
// ---------------------------------------------------------------------------
// Sauvegarde complète de settings.json via Settings::writeFile()
// ---------------------------------------------------------------------------
void WebHandler::handleSave()
{
    CC_LOG_INFO("[Settings][CC] Sauvegarde settings.json (servos + booster + params)\n");

    // Sauvegarde JSON 2026
    Settings::writeFile(canton);

    CC_LOG_INFO("[Settings][CC] settings.json sauvegardé avec succès\n");
}

// ---------------------------------------------------------------------------
// handleRestart()
// ---------------------------------------------------------------------------
void WebHandler::handleRestart()
{
    CC_LOG_WARN("[Settings][CC] Redémarrage demandé\n");
    ESP.restart();
}
