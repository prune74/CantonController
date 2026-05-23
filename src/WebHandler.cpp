/*
   WebHandler.cpp — Version corrigée Discovery 2026
   ------------------------------------------------------------
   - Initialise le serveur HTTP + WebSocket
   - Charge servoCfg[] depuis /settings.json
   - Ne touche plus à Settings::node pour les servos
*/

#include "WebHandler.h"
#include "debug_sa.h"
#include "Settings.h"

#include <SPIFFS.h>
#include <FS.h>
#include <ArduinoJson.h>

// ---------------------------------------------------------------------------
// Constructeur
// ---------------------------------------------------------------------------
WebHandler::WebHandler() :
    _server(nullptr),
    _ws(nullptr),
    node(nullptr)
{
}

// ---------------------------------------------------------------------------
// init()
// ---------------------------------------------------------------------------
void WebHandler::init(Node *node, uint16_t webPort)
{
    this->node = node;

    // Serveur HTTP
    _server = new AsyncWebServer(webPort);

    // WebSocket
    _ws = new AsyncWebSocket("/ws");
    _ws->onEvent(std::bind(&WebHandler::WsEvent, this,
                           std::placeholders::_1,
                           std::placeholders::_2,
                           std::placeholders::_3,
                           std::placeholders::_4,
                           std::placeholders::_5,
                           std::placeholders::_6));

    // Routes HTTP
    route();

    // Ajout WebSocket
    _server->addHandler(_ws);

    // Démarrage serveur
    _server->begin();
    SA_LOG_INFO("[WebHandler] Serveur Web démarré sur port %u\n", webPort);

    // ------------------------------------------------------------
    // Chargement des réglages servo depuis /settings.json
    // ------------------------------------------------------------
    if (!SPIFFS.begin(true))
    {
        SA_LOG_ERROR("[WebHandler] SPIFFS indisponible → valeurs par défaut\n");
        for (uint8_t i = 0; i < 6; ++i)
        {
            servoCfg[i].posDroit = 1500;
            servoCfg[i].posDevie = 1600;
            servoCfg[i].speed    = 5000;
        }
        return;
    }

    File file = SPIFFS.open("/settings.json", "r");
    if (!file)
    {
        SA_LOG_WARN("[WebHandler] settings.json introuvable → valeurs par défaut\n");
        for (uint8_t i = 0; i < 6; ++i)
        {
            servoCfg[i].posDroit = 1500;
            servoCfg[i].posDevie = 1600;
            servoCfg[i].speed    = 5000;
        }
        return;
    }

    StaticJsonDocument<2048> doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error)
    {
        SA_LOG_ERROR("[WebHandler] Erreur JSON → valeurs par défaut : %s\n", error.c_str());
        for (uint8_t i = 0; i < 6; ++i)
        {
            servoCfg[i].posDroit = 1500;
            servoCfg[i].posDevie = 1600;
            servoCfg[i].speed    = 5000;
        }
        return;
    }

    // Lecture réelle des valeurs
    for (uint8_t i = 0; i < 6; ++i)
    {
        char keyPosDroit[16];
        char keyPosDevie[16];
        char keySpeed[16];

        snprintf(keyPosDroit, sizeof(keyPosDroit), "aig%uposDroit", i);
        snprintf(keyPosDevie, sizeof(keyPosDevie), "aig%uposDevie", i);
        snprintf(keySpeed,    sizeof(keySpeed),    "aig%uspeed",    i);

        servoCfg[i].posDroit = doc[keyPosDroit] | 1500;
        servoCfg[i].posDevie = doc[keyPosDevie] | 1600;
        servoCfg[i].speed    = doc[keySpeed]    | 5000;

        SA_LOG_TRACE("[WebHandler] servoCfg[%u] = D:%u  V:%u  S:%u\n",
                     i,
                     servoCfg[i].posDroit,
                     servoCfg[i].posDevie,
                     servoCfg[i].speed);
    }
}

// ---------------------------------------------------------------------------
// loop()
// ---------------------------------------------------------------------------
void WebHandler::loop()
{
    if (_ws)
        _ws->cleanupClients();
}
