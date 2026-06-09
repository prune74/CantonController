/*
   WebHandler.cpp — Exploration 2026 (Option B — Modulaire)
*/

#include "WebHandler.h"
#include "debug_sa.h"
#include "Settings.h"
#include "Booster.h"

WebHandler::WebHandler()
{
    _server = nullptr;
    _ws = nullptr;
    canton = nullptr;

    // Init servoCfg
    for (uint8_t i = 0; i < 6; i++)
    {
        servoCfg[i].posDroit = 1500;
        servoCfg[i].posDevie = 1500;
        servoCfg[i].speed = 5; // slider 0–10
    }
}

void WebHandler::init(Canton *n, uint16_t webPort)
{
    canton = n;

    _server = new AsyncWebServer(webPort);
    _ws = new AsyncWebSocket("/ws");

    _ws->onEvent([this](AsyncWebSocket *server,
                        AsyncWebSocketClient *client,
                        AwsEventType type,
                        void *arg,
                        uint8_t *data,
                        size_t len)
                 { this->WsEvent(server, client, type, arg, data, len); });

    _server->addHandler(_ws);

    route(); // défini dans WebHandler_Routes.cpp

    _server->begin();

    SA_LOG_INFO("[WebHandler] Serveur Web démarré sur port %u\n", webPort);
}

void WebHandler::loop()
{
    if (!_ws || _ws->count() == 0)
        return;

    StaticJsonDocument<256> doc;

    doc["booster_tension"] = Booster::tension();
    doc["booster_courant"] = Booster::courant();
    doc["booster_etat"] = Booster::etat();
    doc["booster_present"] = Booster::present();
    doc["booster_seuil_libre"] = Booster::seuilLibre();
    doc["booster_seuil_occupe"] = Booster::seuilOccupe();

    String out;
    serializeJson(doc, out);
    _ws->textAll(out);
}
