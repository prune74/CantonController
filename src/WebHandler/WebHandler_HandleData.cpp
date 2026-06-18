/*
 * WebHandler_HandleData.cpp — Gestion WebSocket CC
 * ---------------------------------------------------------------------------
 * Analyse et dispatch des messages WebSocket reçus par le CC.
 *
 * Rôle :
 *   - décoder le JSON WebSocket
 *   - router vers les handlers spécialisés :
 *        • Aiguilles (servoSettings / servoTest)
 *        • WiFi
 *        • Exploration
 *        • maxSpeed
 *        • save / restart
 *        • Booster (seuils / calibration)
 *
 * Ce module ne contient aucune logique métier :
 *   → il distribue simplement les commandes WebSocket.
 */

#include "WebHandler.h"
#include "debug_cc.h"
#include "Settings.h"
#include "EXCC_Link.h"
#include "Booster.h"

// ---------------------------------------------------------------------------
// handleWebSocketData()
// ---------------------------------------------------------------------------
void WebHandler::handleWebSocketData(AsyncWebSocketClient *client,
                                     uint8_t *data,
                                     size_t len)
{
    StaticJsonDocument<1024> doc;
    DeserializationError err = deserializeJson(doc, data, len);

    if (err)
    {
        CC_LOG_WARN("[WebHandler][CC] JSON invalide : %s\n", err.c_str());
        return;
    }

    // -----------------------------------------------------------------------
    // AIGUILLES — CONFIGURATION
    // -----------------------------------------------------------------------
    if (doc.containsKey("servoSettings"))
    {
        handleServoSettings(doc);
        notifyClients();
        return;
    }

    // -----------------------------------------------------------------------
    // AIGUILLES — TEST
    // -----------------------------------------------------------------------
    if (doc.containsKey("servoTest"))
    {
        handleServoTest(doc);
        return;
    }

    // -----------------------------------------------------------------------
    // WIFI
    // -----------------------------------------------------------------------
    if (doc.containsKey("wifi_on"))
    {
        handleWifi(doc);
        notifyClients();
        return;
    }

    // -----------------------------------------------------------------------
    // EXPLORATION
    // -----------------------------------------------------------------------
    if (doc.containsKey("exploration_on"))
    {
        handleExploration(doc);
        notifyClients();
        return;
    }

    // -----------------------------------------------------------------------
    // MAX SPEED
    // -----------------------------------------------------------------------
    if (doc.containsKey("maxSpeed"))
    {
        uint8_t v = doc["maxSpeed"];
        canton->maxSpeed(v);

        CC_LOG_INFO("[WebHandler][CC] maxSpeed = %u\n", v);
        notifyClients();
        return;
    }

    // -----------------------------------------------------------------------
    // SAVE
    // -----------------------------------------------------------------------
    if (doc.containsKey("save"))
    {
        handleSave();
        return;
    }

    // -----------------------------------------------------------------------
    // RESTART
    // -----------------------------------------------------------------------
    if (doc.containsKey("restartEsp"))
    {
        handleRestart();
        return;
    }

    // -----------------------------------------------------------------------
    // BOOSTER — seuils manuels
    // -----------------------------------------------------------------------
    if (doc.containsKey("booster_seuils"))
    {
        JsonArray arr = doc["booster_seuils"];
        if (arr.size() >= 2)
        {
            uint16_t libre  = arr[0];
            uint16_t occupe = arr[1];

            // Mise à jour Settings
            Settings::setBoosterSeuilLibre(libre);
            Settings::setBoosterSeuilOccupe(occupe);

            // Sauvegarde JSON
            Settings::writeFile(canton);

            // Mise à jour interne
            Booster::setSeuils(libre, occupe);

            CC_LOG_INFO("[WebHandler][CC] Booster seuils mis à jour : libre=%u occupe=%u\n",
                        libre, occupe);
        }

        notifyClients();
        return;
    }

    // -----------------------------------------------------------------------
    // BOOSTER — calibration auto
    // -----------------------------------------------------------------------
    if (doc.containsKey("cmd") && strcmp(doc["cmd"], "calibBooster") == 0)
    {
        // Plus d’index → un seul EXCC
        EXCC_Link::demanderRecalibration();
        return;
    }

    // -----------------------------------------------------------------------
    // COMMANDE INCONNUE
    // -----------------------------------------------------------------------
    CC_LOG_WARN("[WebHandler][CC] Commande WebSocket inconnue\n");
}
