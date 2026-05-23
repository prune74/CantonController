/*
   WebHandler_HandleData.cpp
   ------------------------------------------------------------
   Analyse et dispatch des messages WebSocket reçus par le SA.

   Ce fichier contient la fonction handleWebSocketData(), chargée de :
     - parser le JSON reçu depuis l’interface Web
     - identifier le type de commande (servoSettings, wifi_on, setRole…)
     - rediriger vers les fonctions spécialisées du module WebHandler

   Le but de ce découpage est de séparer clairement :
     - la réception brute des données (WsEvent)
     - l’analyse du JSON (ce fichier)
     - la logique métier (fichiers spécialisés)
*/

#include "WebHandler.h"
#include "debug_sa.h"
#include "Settings.h"
#include "Discovery.h"

// ---------------------------------------------------------------------------
// handleWebSocketData()
// ---------------------------------------------------------------------------
// Analyse le message JSON reçu via WebSocket et redirige vers la fonction
// spécialisée correspondante.
//
// Paramètres :
//   - client : client WebSocket ayant envoyé les données
//   - data   : buffer contenant les données JSON
//   - len    : taille du buffer
//
// Rôle :
//   - parser le JSON
//   - détecter le type de commande
//   - appeler la fonction métier appropriée
//
// Cette fonction ne contient aucune logique métier : elle se contente
// d’identifier la commande et de déléguer.
// ---------------------------------------------------------------------------
void WebHandler::handleWebSocketData(AsyncWebSocketClient *client,
                                     uint8_t *data,
                                     size_t len)
{
    // Tentative de parsing JSON
    StaticJsonDocument<4066> doc;
    DeserializationError error = deserializeJson(doc, data, len);

    if (error)
    {
        SA_LOG_WARN("[WebHandler] JSON parsing failed: %s\n", error.c_str());
        return;
    }

    // Conversion en String pour faciliter la détection des commandes
    String message = (char *)data;

    SA_LOG_TRACE("[WebHandler] Commande reçue : %s\n", message.c_str());

    // -----------------------------------------------------------------------
    // AIGUILLES — CONFIGURATION
    // -----------------------------------------------------------------------
    if (message.indexOf("servoSettings") >= 0)
    {
        handleServoSettings(doc);
        return;
    }

    // -----------------------------------------------------------------------
    // AIGUILLES — TEST
    // -----------------------------------------------------------------------
    if (message.indexOf("servoTest") >= 0)
    {
        handleServoTest(doc);
        return;
    }

    // -----------------------------------------------------------------------
    // WIFI
    // -----------------------------------------------------------------------
    if (message.indexOf("wifi_on") >= 0)
    {
        handleWifi(doc);
        return;
    }

    // -----------------------------------------------------------------------
    // DISCOVERY
    // -----------------------------------------------------------------------
    if (message.indexOf("discovery_on") >= 0)
    {
        handleDiscovery(doc);
        return;
    }

    // -----------------------------------------------------------------------
    // MAX SPEED
    // -----------------------------------------------------------------------
    if (message.indexOf("maxSpeed") >= 0)
    {
        uint8_t maxSpeed = doc["maxSpeed"][0];
        SA_LOG_INFO("[WebHandler] maxSpeed = %u\n", maxSpeed);
        node->maxSpeed(maxSpeed);
        return;
    }

    // -----------------------------------------------------------------------
    // SAVE
    // -----------------------------------------------------------------------
    if (message.indexOf("save") >= 0)
    {
        handleSave();
        return;
    }

    // -----------------------------------------------------------------------
    // RESTART
    // -----------------------------------------------------------------------
    if (message.indexOf("restartEsp") >= 0)
    {
        handleRestart();
        return;
    }

    // -----------------------------------------------------------------------
    // RÔLE FERROVIAIRE
    // -----------------------------------------------------------------------
    if (message.indexOf("setRole") >= 0)
    {
        handleRole(doc);
        return;
    }

    // -----------------------------------------------------------------------
    // COMMANDE INCONNUE
    // -----------------------------------------------------------------------
    SA_LOG_WARN("[WebHandler] Commande WebSocket inconnue : %s\n",
                message.c_str());
}
