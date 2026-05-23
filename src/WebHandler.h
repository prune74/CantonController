/*
   WebHandler.h
   ------------------------------------------------------------
   Gestion centralisée des interactions WebSocket et HTTP pour le
   Signal Automate (SA). Ce header déclare l’ensemble des méthodes
   utilisées dans les fichiers WebHandler_*.cpp.

   Le but de ce découpage est de rendre le code plus lisible,
   plus modulaire et plus facile à maintenir. Chaque fichier .cpp
   traite une responsabilité unique (routes, WebSocket, aiguilles,
   settings, rôle, etc.).
*/

#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#include "Node.h"          // Structure logique du canton
#include "Aig.h"
#include "debug_sa.h"      // Système de logs Discovery 2026

class WebHandler
{
public:
    // ------------------------------------------------------------
    // Constructeur
    // Initialise les pointeurs internes à nullptr.
    // ------------------------------------------------------------
    WebHandler();

    // ------------------------------------------------------------
    // init()
    // Initialise le serveur Web et le WebSocket.
    // Configure les routes HTTP et démarre le serveur.
    //
    // Paramètres :
    //   - node     : pointeur vers la structure logique du SA
    //   - webPort  : port HTTP à utiliser (ex : 80)
    // ------------------------------------------------------------
    void init(Node *node, uint16_t webPort);

    // ------------------------------------------------------------
    // loop()
    // Fonction appelée régulièrement dans la boucle principale.
    // Permet de nettoyer les clients WebSocket inactifs.
    // ------------------------------------------------------------
    void loop();

private:
    AsyncWebServer     *_server;   // Serveur HTTP
    AsyncWebSocket     *_ws;       // WebSocket principal
    Node               *node;      // Structure logique du SA

    // ------------------------------------------------------------
    // Configuration interne des servos (mémoire centrale du SA)
    // ------------------------------------------------------------
    struct ServoConfig {
        uint16_t posDroit;   // Position droite (µs)
        uint16_t posDevie;   // Position déviée (µs)
        uint16_t speed;      // Vitesse (µs/s)
    };

    // 6 servos maximum : 0-2 = EXSA Horaire, 3-5 = EXSA Antihoraire
    ServoConfig servoCfg[6];

    // ------------------------------------------------------------
    // route()
    // Déclare toutes les routes HTTP (fichiers statiques, API).
    // ------------------------------------------------------------
    void route();

    // ------------------------------------------------------------
    // WsEvent()
    // Gestion des événements WebSocket (connexion, déconnexion,
    // réception de données, erreurs, etc.).
    // ------------------------------------------------------------
    void WsEvent(AsyncWebSocket *server,
                 AsyncWebSocketClient *client,
                 AwsEventType type,
                 void *arg,
                 uint8_t *data,
                 size_t len);

    // ------------------------------------------------------------
    // handleWebSocketData()
    // Analyse le message JSON reçu et redirige vers les fonctions
    // spécialisées (aiguilles, settings, rôle, etc.).
    // ------------------------------------------------------------
    void handleWebSocketData(AsyncWebSocketClient *client,
                             uint8_t *data,
                             size_t len);

    // ------------------------------------------------------------
    // notifyClients()
    // Envoie à tous les clients WebSocket l’état complet du SA :
    // - ID du nœud
    // - connexions P00/P01/P10/P11/M00/M01/M10/M11
    // - aiguilles
    // - paramètres (wifi, discovery, vitesse, rôle, signaux)
    // ------------------------------------------------------------
    void notifyClients();

    // ------------------------------------------------------------
    // Fonctions spécialisées pour chaque type de commande Web
    // ------------------------------------------------------------

    // Aiguilles
    void handleServoSettings(JsonDocument &doc);
    void handleServoTest(JsonDocument &doc);

    // Paramètres (wifi, discovery, save, restart)
    void handleWifi(JsonDocument &doc);
    void handleDiscovery(JsonDocument &doc);
    void handleSave();
    void handleRestart();

    // Rôle ferroviaire
    void handleRole(JsonDocument &doc);
};
