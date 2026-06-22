/*
 * WebHandler_Routes.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Déclaration des routes HTTP du serveur embarqué :
 *   - page principale (index.html)
 *   - fichiers statiques (CSS / JS / images / JSON)
 *   - settings.json (lecture seule)
 *   - gestion des erreurs 404
 *
 * Ce module ne contient aucune logique métier :
 *   → il expose simplement les ressources Web du CC.
 */

#include "WebHandler.h"
#include "debug_cc.h"
#include <SPIFFS.h>

// ---------------------------------------------------------------------------
// route()
// ---------------------------------------------------------------------------
void WebHandler::route() // 🟢
{
    // -----------------------------------------------------------------------
    // Page principale
    // -----------------------------------------------------------------------
    _server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/index.html", "text/html");
    });

    // -----------------------------------------------------------------------
    // Fichiers statiques (CSS / JS / images / JSON)
    // -----------------------------------------------------------------------
    _server->serveStatic("/", SPIFFS, "/");

    // -----------------------------------------------------------------------
    // settings.json (lecture seule)
    // -----------------------------------------------------------------------
    _server->on("/settings.json", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/settings.json", "application/json");
    });

    // -----------------------------------------------------------------------
    // Gestion des erreurs 404
    // -----------------------------------------------------------------------
    _server->onNotFound([](AsyncWebServerRequest *request)
    {
        CC_LOG_WARN("[HTTP][CC] 404 Not Found : %s\n",
                    request->url().c_str());

        request->send(404, "text/plain", "404 - Not Found");
    });
}
