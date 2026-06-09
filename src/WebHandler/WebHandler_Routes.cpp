/*
   WebHandler_Routes.cpp — Exploration 2026 (FINAL & CLEAN)
*/

#include "WebHandler.h"
#include "debug_sa.h"
#include <SPIFFS.h>

// ---------------------------------------------------------------------------
// route()
// ---------------------------------------------------------------------------
void WebHandler::route()
{
   // -----------------------------------------------------------------------
   // Page principale
   // -----------------------------------------------------------------------
   _server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(SPIFFS, "/index.html", "text/html"); });

   // -----------------------------------------------------------------------
   // Fichiers statiques (CSS / JS / images / JSON)
   // -----------------------------------------------------------------------
   _server->serveStatic("/", SPIFFS, "/");

   // -----------------------------------------------------------------------
   // settings.json (lecture seule)
   // -----------------------------------------------------------------------
   _server->on("/settings.json", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(SPIFFS, "/settings.json", "application/json"); });

   // -----------------------------------------------------------------------
   // Gestion des erreurs 404
   // -----------------------------------------------------------------------
   _server->onNotFound([](AsyncWebServerRequest *request)
                       {
        SA_LOG_WARN("[HTTP] 404 Not Found : %s\n",
                    request->url().c_str());

        request->send(404, "text/plain", "404 - Not Found"); });
}
