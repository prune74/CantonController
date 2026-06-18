/*
 * WebHandler.cpp — Gestion Canton 2026
 * ------------------------------------------------------------
 * Gestion du serveur Web et du WebSocket pour le Canton Controller (CC).
 *
 * Le CC expose une interface Web permettant :
 *   - la configuration des aiguilles
 *   - la gestion du booster
 *   - la configuration générale (WiFi, exploration, rôle)
 *   - la visualisation en temps réel de l’état du canton
 *
 * Le WebHandler assure :
 *   - l’initialisation du serveur HTTP
 *   - la gestion du WebSocket /ws
 *   - l’envoi périodique des états (booster, aiguilles…)
 *   - le dispatch des commandes reçues (via WebHandler_HandleData.cpp)
 */

#include "WebHandler.h"
#include "debug_cc.h"
#include "Settings.h"
#include "Booster.h"

// ---------------------------------------------------------------------------
// Constructeur
// ---------------------------------------------------------------------------
WebHandler::WebHandler()
{
    _server = nullptr;
    _ws     = nullptr;
    canton  = nullptr;

    // Initialisation des paramètres servo (positions par défaut)
    for (uint8_t i = 0; i < aigSize; i++)
    {
        servoCfg[i].posDroit = 1500;
        servoCfg[i].posDevie = 1500;
        servoCfg[i].speed    = 5;   // slider 0–10
    }
}

// ---------------------------------------------------------------------------
// init()
// Initialise le serveur HTTP + WebSocket
// ---------------------------------------------------------------------------
void WebHandler::init(Canton *n, uint16_t webPort)
{
    canton = n;

    _server = new AsyncWebServer(webPort);
    _ws     = new AsyncWebSocket("/ws");

    // Callback WebSocket
    _ws->onEvent([this](AsyncWebSocket *server,
                        AsyncWebSocketClient *client,
                        AwsEventType type,
                        void *arg,
                        uint8_t *data,
                        size_t len)
    {
        this->WsEvent(server, client, type, arg, data, len);
    });

    _server->addHandler(_ws);

    // Définition des routes HTTP (voir WebHandler_Routes.cpp)
    route();

    _server->begin();

    CC_LOG_INFO("[WebHandler][CC] Serveur Web démarré sur port %u\n", webPort);
}

// ---------------------------------------------------------------------------
// loop()
// Envoi périodique des informations Booster aux clients WebSocket
// ---------------------------------------------------------------------------
void WebHandler::loop()
{
    if (!_ws || _ws->count() == 0)
        return;

    StaticJsonDocument<256> doc;

    doc["booster_tension"]      = Booster::tension();
    doc["booster_courant"]      = Booster::courant();
    doc["booster_etat"]         = Booster::etat();
    doc["booster_present"]      = Booster::present();
    doc["booster_seuil_libre"]  = Booster::seuilLibre();
    doc["booster_seuil_occupe"] = Booster::seuilOccupe();

    String out;
    serializeJson(doc, out);
    _ws->textAll(out);
}
