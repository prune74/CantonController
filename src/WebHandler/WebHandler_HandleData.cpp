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
 *        • Pilotage Distribué (longueur / zones / écarts)
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
void WebHandler::handleWebSocketData(AsyncWebSocketClient *client, uint8_t *data, size_t len)
{
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, data, len);

    if (err)
    {
        CC_LOG_WARN("[WebHandler][CC] JSON invalide : %s\n", err.c_str());
        return;
    }

    // -----------------------------------------------------------------------
    // AIGUILLES — CONFIGURATION
    // -----------------------------------------------------------------------
    {
        JsonVariant v = doc["servoSettings"];
        if (!v.isNull())
        {
            handleServoSettings(doc);
            notifyClients();
            return;
        }
    }

    // -----------------------------------------------------------------------
    // AIGUILLES — TEST
    // -----------------------------------------------------------------------
    {
        JsonVariant v = doc["servoTest"];
        if (!v.isNull())
        {
            handleServoTest(doc);
            return;
        }
    }

    // -----------------------------------------------------------------------
    // WIFI
    // -----------------------------------------------------------------------
    {
        JsonVariant v = doc["wifi_on"];
        if (!v.isNull())
        {
            handleWifi(doc);
            notifyClients();
            return;
        }
    }

    // -----------------------------------------------------------------------
    // EXPLORATION
    // -----------------------------------------------------------------------
    {
        JsonVariant v = doc["exploration_on"];
        if (!v.isNull())
        {
            handleExploration(doc);
            notifyClients();
            return;
        }
    }

    // -----------------------------------------------------------------------
    // MAX SPEED
    // -----------------------------------------------------------------------
    {
        JsonVariant v = doc["maxSpeed"];
        if (!v.isNull())
        {
            uint8_t val = v | 0;
            canton->maxSpeed(val);

            CC_LOG_INFO("[WebHandler][CC] maxSpeed = %u\n", val);
            notifyClients();
            return;
        }
    }

    // -----------------------------------------------------------------------
    // SAVE
    // -----------------------------------------------------------------------
    {
        JsonVariant v = doc["save"];
        if (!v.isNull())
        {
            handleSave();
            return;
        }
    }

    // -----------------------------------------------------------------------
    // RESTART
    // -----------------------------------------------------------------------
    {
        JsonVariant v = doc["restartEsp"];
        if (!v.isNull())
        {
            handleRestart();
            return;
        }
    }

    // -----------------------------------------------------------------------
    // BOOSTER — seuils manuels
    // -----------------------------------------------------------------------
    {
        JsonVariant v = doc["booster_seuils"];
        if (!v.isNull())
        {
            JsonArray arr = v.as<JsonArray>();
            if (arr.size() >= 2)
            {
                uint16_t libre  = arr[0] | 0;
                uint16_t occupe = arr[1] | 0;

                Settings::setBoosterSeuilLibre(libre);
                Settings::setBoosterSeuilOccupe(occupe);

                Settings::writeFile(canton);
                Booster::setSeuils(libre, occupe);

                CC_LOG_INFO("[WebHandler][CC] Booster seuils mis à jour : libre=%u occupe=%u\n",
                            libre, occupe);
            }

            notifyClients();
            return;
        }
    }

    // -----------------------------------------------------------------------
    // BOOSTER — calibration auto
    // -----------------------------------------------------------------------
    {
        JsonVariant v = doc["cmd"];
        if (!v.isNull())
        {
            const char *cmd = v.as<const char*>();
            if (cmd && strcmp(cmd, "calibBooster") == 0)
            {
                EXCC_Link::demanderRecalibration();
                return;
            }
        }
    }

    // -----------------------------------------------------------------------
    // PILOTAGE DISTRIBUE — longueur / zones / écarts
    // -----------------------------------------------------------------------
    if (isPilotageDistribue(doc))
    {
        handlePilotageDistribue(doc);
        return;
    }

    // -----------------------------------------------------------------------
    // COMMANDE INCONNUE
    // -----------------------------------------------------------------------
    CC_LOG_WARN("[WebHandler][CC] Commande WebSocket inconnue\n");
}
