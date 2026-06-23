/*
 * WebHandler.h — Gestion Canton 2026
 * ------------------------------------------------------------
 * Gestion centralisée du serveur HTTP et du WebSocket pour le
 * Canton Controller (CC).
 *
 * Le WebHandler assure :
 *   - l’initialisation du serveur Web
 *   - la gestion du WebSocket /ws
 *   - la réception et le dispatch des commandes JSON
 *   - l’envoi périodique de l’état du booster
 *   - la notification complète de l’état du canton
 *
 * Les handlers spécialisés sont répartis dans :
 *   - WebHandler_HandleData.cpp
 *   - WebHandler_Aiguilles.cpp
 *   - WebHandler_Settings.cpp
 *   - WebHandler_Notify.cpp
 */

#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#include "Canton.h"
#include "Aig.h"
#include "debug_cc.h"

class WebHandler
{
public:
    // -----------------------------------------------------------------------
    // Constructeur
    // -----------------------------------------------------------------------
    WebHandler();

    // -----------------------------------------------------------------------
    // init()
    // Initialise le serveur HTTP + WebSocket
    // -----------------------------------------------------------------------
    void init(Canton *canton, uint16_t webPort);

    // -----------------------------------------------------------------------
    // loop()
    // Envoi périodique des informations Booster aux clients WebSocket
    // -----------------------------------------------------------------------
    void loop();

private:
    AsyncWebServer *_server;   // Serveur HTTP
    AsyncWebSocket *_ws;       // WebSocket principal
    Canton *canton;            // Référence vers le Canton Controller (CC)

    // -----------------------------------------------------------------------
    // Déclaration des sous‑modules
    // -----------------------------------------------------------------------
    void route();  // Routes HTTP (défini dans WebHandler_Routes.cpp)

    // Gestion des événements WebSocket
    void WsEvent(AsyncWebSocket *server,
                 AsyncWebSocketClient *client,
                 AwsEventType type,
                 void *arg,
                 uint8_t *data,
                 size_t len);

    // Analyse JSON → dispatch vers les handlers spécialisés
    void handleWebSocketData(AsyncWebSocketClient *client,
                             uint8_t *data,
                             size_t len);

    // Envoi de l’état complet du canton (CC) aux clients WebSocket
    void notifyClients();

    // Envoi de l’état du Booster à un client lors de la connexion
    void sendBoosterState(AsyncWebSocketClient *client);

    // -----------------------------------------------------------------------
    // HANDLERS SPÉCIALISÉS
    // -----------------------------------------------------------------------

    // Aiguilles
    void handleServoSettings(JsonDocument &doc);
    void handleServoTest(JsonDocument &doc);

    // Paramètres généraux
    void handleWifi(JsonDocument &doc);
    void handleExploration(JsonDocument &doc);
    void handleSave();
    void handleRestart();

};
