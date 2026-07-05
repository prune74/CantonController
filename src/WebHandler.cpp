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
    if (!_ws)
        return;

    // Nettoyage des clients fantômes
    _ws->cleanupClients();

    // Si aucun client → on n’envoie rien
    if (_ws->count() == 0)
        return;

    // Limiteur de fréquence (20 Hz)
    static uint32_t lastSend = 0;
    uint32_t now = millis();
    if (now - lastSend < 50)
        return;
    lastSend = now;

    // Vérification des buffers
    for (size_t i = 0; i < _ws->count(); i++) {
        AsyncWebSocketClient *c = _ws->client(i);
        if (!c) {
            return; // client fantôme
        }
        if (c->queueIsFull()) {
            return; // client saturé → on n’envoie rien
        }
    }

    JsonDocument doc;

    doc["booster_tension"]      = Booster::tension();
    doc["booster_courant"]      = Booster::courant();
    doc["booster_etat"]         = Booster::etat();
    doc["booster_seuil_libre"]  = Booster::seuilLibre();
    doc["booster_seuil_occupe"] = Booster::seuilOccupe();

    String out;
    serializeJson(doc, out);

    // Envoi sécurisé
    _ws->textAll(out);

    CC_LOG_TRACE("[WebHandler][CC] Booster → état envoyé aux clients WebSocket\n");
}