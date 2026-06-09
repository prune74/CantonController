/*
   WebHandler_HandleData.cpp — Exploration 2026 (CLEAN & FIXED)
   ------------------------------------------------------------
   Analyse et dispatch des messages WebSocket reçus par le SA.
*/

#include "WebHandler.h"
#include "debug_sa.h"
#include "Settings.h"
#include "SatEXSA_Link.h"
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
        SA_LOG_WARN("[WebHandler] JSON invalide : %s\n", err.c_str());
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
        SA_LOG_INFO("[WebHandler] maxSpeed = %u\n", v);
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
    // RÔLE FERROVIAIRE
    // -----------------------------------------------------------------------
    if (doc.containsKey("cmd") && strcmp(doc["cmd"], "setRole") == 0)
    {
        handleRole(doc);
        return;
    }

    // -----------------------------------------------------------------------
    // BOOSTER — seuils
    // -----------------------------------------------------------------------
    if (doc.containsKey("booster_seuils"))
    {
        JsonArray arr = doc["booster_seuils"];
        if (arr.size() >= 2)
        {
            uint16_t libre = arr[0];
            uint16_t occupe = arr[1];

            Settings::setBoosterSeuilLibre(libre);
            Settings::setBoosterSeuilOccupe(occupe);
            Settings::save();

            Booster::setSeuils(libre, occupe);

            SA_LOG_INFO("[WebHandler] Booster seuils mis à jour : libre=%u occupe=%u\n",
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
        int8_t idx = SatEXSA_Link::getBoosterExsaIndex();
        if (idx >= 0)
            SatEXSA_Link::demanderRecalibration(idx);
        else
            SA_LOG_WARN("[WebHandler] Calibration demandée mais aucun EXSA booster détecté\n");

        return;
    }

    // -----------------------------------------------------------------------
    // COMMANDE INCONNUE
    // -----------------------------------------------------------------------
    SA_LOG_WARN("[WebHandler] Commande WebSocket inconnue\n");
}
