/*
   WebHandler.cpp - Gestion des interactions WebSocket et HTTP
   Version 2026 — Le SA ne pilote plus les servos localement.
   Toute commande d’aiguille est transmise à EXSA via RS485.
*/

#include "WebHandler.h"
#include "SatTopologieUART.h"   // envoyerServoConfig / envoyerServoTest

WebHandler::WebHandler() : _server(nullptr), _ws(nullptr) {}

void WebHandler::init(Canton *canton, uint16_t webPort)
{
  _server = new AsyncWebServer(webPort);
  _ws = new AsyncWebSocket("/ws");
  _ws->onEvent(std::bind(&WebHandler::WsEvent, this,
                         std::placeholders::_1,
                         std::placeholders::_2,
                         std::placeholders::_3,
                         std::placeholders::_4,
                         std::placeholders::_5,
                         std::placeholders::_6));

  WebHandler::route();

  _server->addHandler(_ws);
  _server->begin();

  this->canton = canton;
}

void WebHandler::loop()
{
  _ws->cleanupClients();
}

void WebHandler::WsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                         AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
#ifdef DEBUG
    debug.printf("WebSocket client #%u connected from %s\n",
                 client->id(), client->remoteIP().toString().c_str());
#endif
    WebHandler::notifyClients();
    break;

  case WS_EVT_DISCONNECT:
#ifdef DEBUG
    debug.printf("WebSocket client #%u disconnected\n", client->id());
#endif
    break;

  case WS_EVT_ERROR:
  case WS_EVT_PONG:
    break;

  case WS_EVT_DATA:

#ifdef DEBUG
    debug.printf("[WebHandler %d] : WebSocket len %d\n", __LINE__, len);
#endif

    StaticJsonDocument<4066> doc1;
    DeserializationError error = deserializeJson(doc1, data);

    if (error)
    {
#ifdef DEBUG
      debug.println("Parsing failed");
#endif
      return;
    }

    String message = (char *)data;

    // -------------------------
    // AIGUILLES — CONFIGURATION
    // -------------------------
    if (message.indexOf("servoSettings") >= 0)
    {
      const char *servoId = doc1["servoSettings"][0];
      const uint16_t servoValue = doc1["servoSettings"][1];
      const uint8_t servoName = doc1["servoSettings"][2];

      // Mise à jour logique interne
      if ('0' == servoId[2])      // Position droite
        canton->aig[servoName]->posDroit(servoValue);
      else if ('1' == servoId[2]) // Position déviée
        canton->aig[servoName]->posDevie(servoValue);
      else if ('2' == servoId[2]) // Vitesse
      {
        uint16_t speed = 11000 - (servoValue * 1000);
        canton->aig[servoName]->speed(speed);
      }

      // Déterminer EXSA H/AH
      uint8_t exsaAdresse =
        (canton->aig[servoName]->cantonPdroitIdx() == canton->SP1_idx()) ? 0 : 1;

      // Envoi F1 : servoConfig (adressé)
      envoyerServoConfig(exsaAdresse,
                         servoName,
                         canton->aig[servoName]->posDroit(),
                         canton->aig[servoName]->posDevie(),
                         canton->aig[servoName]->speed());
    }

    // -------------------------
    // AIGUILLES — TEST
    // -------------------------
    if (message.indexOf("servoTest") >= 0)
    {
      const uint8_t servoName = doc1["servoTest"][0];

      // Déterminer EXSA H/AH
      uint8_t exsaAdresse =
        (canton->aig[servoName]->cantonPdroitIdx() == canton->SP1_idx()) ? 0 : 1;

      // Envoi F2 : servoTest (adressé)
      envoyerServoTest(exsaAdresse, servoName);
    }

    // -------------------------
    // WIFI
    // -------------------------
    if (message.indexOf("wifi_on") >= 0)
    {
      const bool wifi_on = doc1["wifi_on"][0];
      Settings::wifiOn(wifi_on);
      Settings::writeFile();
      delay(1000);
      ESP.restart();
    }

    // -------------------------
    // EXPLORATION
    // -------------------------
    if (message.indexOf("exploration_on") >= 0)
    {
      const bool exploration_on = doc1["exploration_on"][0];
      Settings::explorationOn(exploration_on);
      if (!exploration_on)
        Exploration::stopProcess(true);
      Settings::writeFile();
    }

    // -------------------------
    // MAX SPEED
    // -------------------------
    if (message.indexOf("maxSpeed") >= 0)
    {
      const uint8_t maxSpeed = doc1["maxSpeed"][0];
      canton->maxSpeed(maxSpeed);
    }

    // -------------------------
    // SAVE
    // -------------------------
    if (message.indexOf("save") >= 0)
    {
      Settings::writeFile();
    }

    // -------------------------
    // RESTART
    // -------------------------
    if (message.indexOf("restartEsp") >= 0)
    {
      ESP.restart();
    }

    // -------------------------
    // 🔥 RÔLE FERROVIAIRE
    // -------------------------
    if (message.indexOf("setRole") >= 0)
    {
      const uint8_t role = doc1["setRole"];

      canton->setRole((CantonRole)role);
      Settings::writeFile();

#ifdef DEBUG
      debug.printf("[setRole] Nouveau rôle : %d\n", role);
#endif

      WebHandler::notifyClients();
    }

    break;
  }
}

void WebHandler::notifyClients()
{
  StaticJsonDocument<1024> doc;

  doc["idCanton"] = canton->ID();

  // Nœuds
  String index[] = {"p00", "p01", "p10", "p11", "m00", "m01", "m10", "m11"};
  for (byte i = 0; i < 8; i++)
  {
    if (canton->cantonP[i] == nullptr)
      doc[index[i]] = "null";
    else
      doc[index[i]] = canton->cantonP[i]->ID();
  }

  // Aiguilles
  for (byte i = 0; i < aigSize; i++)
  {
    if (canton->aig[i] == nullptr)
    {
      doc["s" + String(i)] = "null";
      doc["s" + String(i) + "0"] = "";
      doc["s" + String(i) + "1"] = "";
      doc["s" + String(i) + "2"] = "";
    }
    else
    {
      doc["s" + String(i)] = "Actif";
      doc["s" + String(i) + "0"] = canton->aig[i]->posDroit();
      doc["s" + String(i) + "1"] = canton->aig[i]->posDevie();
      doc["s" + String(i) + "2"] = (11000 - canton->aig[i]->speed()) / 1000;
    }
  }

  doc["wifi_on"] = Settings::wifiOn();
  doc["exploration_on"] = Settings::explorationOn();

  doc["maxSpeed"] = canton->maxSpeed();
  doc["sensMarche"] = canton->sensMarche();

  // 🔥 Rôle ferroviaire
  doc["role"] = (uint8_t)canton->role();

  // Signaux
  if (canton->signal[0] != nullptr)
    doc["cibleHoraire"] = canton->signal[0]->type();
  if (canton->signal[1] != nullptr)
    doc["cibleAntiHor"] = canton->signal[1]->type();

  String output;
  serializeJson(doc, output);
  _ws->textAll(output);
}

void WebHandler::route()
{
  _server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html", "text/html"); });

  _server->on("/w3.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/w3.css", "text/css"); });

  _server->on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/style.css", "text/css"); });

  _server->on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/script.js", "text/javascript"); });

  _server->on("/settings.json", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/settings.json", "text/json"); });

  _server->on("/favicon.png", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/favicon.png", "image/png"); });

  _server->on("/cible_0.jpg", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/cible_0.jpg", "image/jpg"); });

  _server->on("/cible_1.jpg", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/cible_1.jpg", "image/jpg"); });

  _server->on("/cible_2.jpg", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/cible_2.jpg", "image/jpg"); });

  _server->on("/cible_3.jpg", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/cible_3.jpg", "image/jpg"); });

  _server->onNotFound([](AsyncWebServerRequest *request)
                      {
                        Serial.printf("Not found: %s!\r\n", request->url().c_str());
                        request->send(404);
                      });
}
