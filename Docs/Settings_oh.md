/*
   Settings.h — Version 2026
   ------------------------------------------------------------
   Rôle de Settings :
   - Gère la configuration persistante du SA (settings.json)
   - Initialise l’UART RS485 (communication SA → EXSA)
   - Dialogue CAN avec la carte Main pour obtenir l’ID du SA
   - Stocke les paramètres globaux (WiFi, Discovery, SSID, etc.)
   - Reconstruit les objets NodePeriph, Aig, Signal à partir du JSON

   IMPORTANT :
   Le SA ne pilote plus les servos localement.
   Settings charge uniquement les valeurs logiques (posDroit, posDevie,
   vitesse, index EXSA H/AH), qui seront utilisées pour envoyer des
   trames RS485 vers EXSA.
   ------------------------------------------------------------
*/

#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <FS.h>
#include "CanMsg.h"
#include "CanConfig.h"
#include "Config.h"
#include "Node.h"

class Settings
{
private:

  // Indique si la carte Main a répondu (CAN 0xB3)
  static bool isMainReady;

  // Identifiants WiFi stockés dans settings.json
  static String ssid_str;
  static String password_str;

  // Paramètres globaux
  static bool WIFI_ON;
  static bool DISCOVERY_ON;

  // Référence vers le Node principal (logique ferroviaire)
  static Node *node;

  // UART RS485 partagé entre EXSA_H et EXSA_AH
  static HardwareSerial SerialUART;

public:

  // Buffers C pour SSID / mot de passe (compatibilité)
  static char ssid[30];
  static char password[30];

  // Constructeur interdit (classe statique)
  Settings() = delete;

  // Initialisation générale (UART + SPIFFS + settings.json)
  static void setup(Node *);

  // Dialogue CAN avec la carte Main (obtention ID)
  static bool begin();

  // Lecture / écriture du fichier settings.json
  static void writeFile();
  static void readFile();

  // Indique que la carte Main est prête
  static void sMainReady(bool);

  // Gestion du mode Discovery
  static bool discoveryOn();
  static void discoveryOn(bool);

  // Gestion du WiFi
  static bool wifiOn();
  static void wifiOn(bool);

  // Accès à l’UART RS485
  static HardwareSerial& uart();
};
