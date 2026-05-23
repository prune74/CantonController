/*
   WebHandler_Routes.cpp
   ------------------------------------------------------------
   Déclaration des routes HTTP utilisées par l’interface Web du
   Signal Automate (SA).

   Ce fichier regroupe toutes les routes permettant de servir :
     - la page principale (index.html)
     - les feuilles de style (CSS)
     - les scripts JavaScript
     - les images (signaux, favicon…)
     - le fichier settings.json

   Le but de ce découpage est de séparer clairement :
     - la logique WebSocket (temps réel)
     - la logique HTTP (fichiers statiques)

   Cela rend WebHandler plus lisible et facilite l’onboarding.
*/

#include "WebHandler.h"
#include "debug_sa.h"
#include <SPIFFS.h>

// ---------------------------------------------------------------------------
// route()
// ---------------------------------------------------------------------------
// Déclare toutes les routes HTTP du serveur Web.
//
// Rôle :
//   - servir les fichiers statiques depuis SPIFFS
//   - gérer les erreurs 404
//
// Cette fonction ne contient aucune logique métier : elle se contente
// d’exposer les fichiers nécessaires à l’interface Web.
// ---------------------------------------------------------------------------
void WebHandler::route()
{
    // -----------------------------------------------------------------------
    // Page principale
    // -----------------------------------------------------------------------
    _server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/index.html", "text/html");
    });

    // -----------------------------------------------------------------------
    // Feuilles de style
    // -----------------------------------------------------------------------
    _server->on("/w3.css", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/w3.css", "text/css");
    });

    _server->on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/style.css", "text/css");
    });

    // -----------------------------------------------------------------------
    // Script JavaScript principal
    // -----------------------------------------------------------------------
    _server->on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/script.js", "text/javascript");
    });

    // -----------------------------------------------------------------------
    // Fichier de configuration
    // -----------------------------------------------------------------------
    _server->on("/settings.json", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/settings.json", "text/json");
    });

    // -----------------------------------------------------------------------
    // Images (signaux ferroviaires)
    // -----------------------------------------------------------------------
    _server->on("/favicon.png", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/favicon.png", "image/png");
    });

    _server->on("/cible_0.jpg", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/cible_0.jpg", "image/jpg");
    });

    _server->on("/cible_1.jpg", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/cible_1.jpg", "image/jpg");
    });

    _server->on("/cible_2.jpg", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/cible_2.jpg", "image/jpg");
    });

    _server->on("/cible_3.jpg", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/cible_3.jpg", "image/jpg");
    });

    // -----------------------------------------------------------------------
    // Gestion des erreurs 404
    // -----------------------------------------------------------------------
    _server->onNotFound([](AsyncWebServerRequest *request)
    {
        SA_LOG_WARN("[HTTP] 404 Not Found : %s\n",
                    request->url().c_str());

        request->send(404);
    });
}
