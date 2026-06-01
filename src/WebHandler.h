/*
   WebHandler.h — Discovery 2026
   ------------------------------------------------------------
   Gestion centralisée des interactions WebSocket et HTTP pour le SA.
   Version alignée avec WebHandler.cpp (fusionné).
*/

#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#include "Node.h"
#include "Aig.h"
#include "debug_sa.h"

class WebHandler
{
public:
    // ------------------------------------------------------------
    // Constructeur
    // ------------------------------------------------------------
    WebHandler();

    // ------------------------------------------------------------
    // init()
    // Initialise serveur HTTP + WebSocket + chargement settings
    // ------------------------------------------------------------
    void init(Node *node, uint16_t webPort);

    // ------------------------------------------------------------
    // loop()
    // Nettoyage WebSocket + envoi périodique Booster
    // ------------------------------------------------------------
    void loop();

private:
    AsyncWebServer     *_server;   // Serveur HTTP
    AsyncWebSocket     *_ws;       // WebSocket principal
    Node               *node;      // Structure logique du SA

    // ------------------------------------------------------------
    // Configuration interne des servos
    // ------------------------------------------------------------
    struct ServoConfig {
        uint16_t posDroit;   // Position droite (µs)
        uint16_t posDevie;   // Position déviée (µs)
        uint16_t speed;      // Vitesse (µs/s)
    };

    ServoConfig servoCfg[6]; // 6 servos max

    // ------------------------------------------------------------
    // ROUTES HTTP
    // ------------------------------------------------------------
    void route();

    // ------------------------------------------------------------
    // WebSocket Event Handler
    // ------------------------------------------------------------
    void WsEvent(AsyncWebSocket *server,
                 AsyncWebSocketClient *client,
                 AwsEventType type,
                 void *arg,
                 uint8_t *data,
                 size_t len);

    // ------------------------------------------------------------
    // Analyse JSON reçu → dispatch vers handlers spécialisés
    // ------------------------------------------------------------
    void handleWebSocketData(AsyncWebSocketClient *client,
                             uint8_t *data,
                             size_t len);

    // ------------------------------------------------------------
    // Envoi état complet SA + Booster à tous les clients
    // ------------------------------------------------------------
    void notifyClients();

    // ------------------------------------------------------------
    // Envoi état Booster à un client (connexion)
    // ------------------------------------------------------------
    void sendBoosterState(AsyncWebSocketClient *client);

    // ------------------------------------------------------------
    // HANDLERS SPÉCIALISÉS
    // ------------------------------------------------------------

    // Aiguilles
    void handleServoSettings(JsonDocument &doc);
    void handleServoTest(JsonDocument &doc);

    // Paramètres généraux
    void handleWifi(JsonDocument &doc);
    void handleDiscovery(JsonDocument &doc);
    void handleSave();
    void handleRestart();

    // Rôle ferroviaire
    void handleRole(JsonDocument &doc);
};
