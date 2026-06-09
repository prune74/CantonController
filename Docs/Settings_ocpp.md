/*
   Settings.cpp - Gestion des paramètres et de la configuration
   ------------------------------------------------------------
   Rôle :
   - Initialise l’UART RS485 (communication SA → EXSA)
   - Charge settings.json (positions aiguilles, signaux, voisins…)
   - Applique le rôle ferroviaire
   - Reconstruit les objets CantonPeriph, Aig, Signal
   - Communique avec la carte Main via CAN pour obtenir l’ID
   ------------------------------------------------------------
*/

#include "Settings.h"

bool Settings::WIFI_ON = true;
bool Settings::EXPLORATION_ON = true;
String Settings::ssid_str;
String Settings::password_str;
char Settings::ssid[30] = {};
char Settings::password[30] = {};
bool Settings::isMainReady = false;
Canton *Settings::canton = nullptr;

// UART RS485 partagé entre EXSA_H et EXSA_AH
HardwareSerial Settings::SerialUART(1);
HardwareSerial& Settings::uart() { return SerialUART; }

void Settings::sMainReady(bool val) { Settings::isMainReady = val; }
bool Settings::explorationOn() { return EXPLORATION_ON; }
void Settings::explorationOn(bool val) { Settings::EXPLORATION_ON = val; }
bool Settings::wifiOn() { return WIFI_ON; }
void Settings::wifiOn(bool val) { Settings::WIFI_ON = val; }

/*-------------------------------------------------------------
                           setup()
   Initialise :
   - UART RS485
   - SPIFFS
   - settings.json
--------------------------------------------------------------*/
void Settings::setup(Canton *nd)
{
  canton = nd;

  // UART RS485 (communication SA → EXSA)
  Settings::uart().begin(UART_BAUDRATE, SERIAL_8N1, UART_RX_SATELLITE, UART_TX_SATELLITE);
  Serial.printf("[Settings %d] : UART initialisé sur UART%d à %lu bauds\n",
                __LINE__, UART_PORT_NUM, UART_BAUDRATE);

  // Montage SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.printf("[Settings %d] : Erreur SPIFFS\n\n", __LINE__);
    return;
  }
  else
    Serial.printf("[Settings %d] : SPIFFS monté\n", __LINE__);

  readFile();
}

/*-------------------------------------------------------------
                           begin()
   Dialogue CAN avec la carte Main :
   - Attente de la réponse CMD_SAT_TEST_BUS_REPLY
   - Demande d’ID (0xB4)
--------------------------------------------------------------*/
bool Settings::begin()
{
  uint8_t countReset = 0;
  Serial.printf("[Settings %d] : Attente de reponse en provenance de la carte Main.\n", __LINE__);

  // Attente du message 0xB3 (Main ready)
  do
  {
    CanMsg::sendMsg(0, 0xB2, 0, canton->ID());
    vTaskDelay(pdMS_TO_TICKS(1000));
    Serial.print(".");
    if (countReset == 10)
    {
      Serial.printf(" \n\n [Settings %d] : *** Redemarrage dans 5 secondes ***\n\n", __LINE__);
      delay(5000);
      esp_restart();
    }
    countReset++;
  } while (!isMainReady);

  // Demande d’ID si non attribué
  if (canton->ID() == UNUSED_ID)
    Serial.printf("\n[Settings %d] : Le satellite ne possede pas d'identifiant.\n", __LINE__);

  while (canton->ID() == UNUSED_ID)
  {
    CanMsg::sendMsg(0, 0xB4, 0, canton->ID());
    vTaskDelay(pdMS_TO_TICKS(1000));
    if (canton->ID() != UNUSED_ID)
      writeFile();
    else
      Serial.print(".");
  }

  Serial.printf("\n[Settings %d] : End settings\n", __LINE__);
  Serial.printf("-----------------------------------\n\n");

  return 0;
}

/*-------------------------------------------------------------
                           readFile()
   Charge settings.json :
   - ID du SA
   - voisins (CantonPeriph)
   - aiguilles (Aig)
   - signaux
   - rôle ferroviaire
   - paramètres WiFi / Exploration
--------------------------------------------------------------*/
void Settings::readFile()
{
  File file = SPIFFS.open("/settings.json", "r");
  if (!file)
  {
#ifdef DEBUG
    debug.printf("[Settings %d] : Failed to open settings.json\n\n", __LINE__);
#endif
    return;
  }

#ifdef DEBUG
  debug.printf("\nInformations du fichier \"settings.json\" : \n\n");
  while (file.available())
    Serial.write(file.read());
  Serial.printf("\n\n");
  file.seek(0);
#endif

  size_t size = file.size();
  if (size > 2 * 1024)
  {
    Serial.println("File size is too large");
    file.close();
    return;
  }

  DynamicJsonDocument doc(2 * 1024);
  DeserializationError error = deserializeJson(doc, file);
  vTaskDelay(pdMS_TO_TICKS(100));

  if (error)
  {
#ifdef DEBUG
    debug.printf("[Settings %d] Failed to read file, using default configuration\n\n", __LINE__);
    debug.printf("[Settings %d] DeserializationError: %s\n", __LINE__, error.c_str());
#endif
    file.close();
    return;
  }

  // ID du SA
  canton->ID(doc["idCanton"] | UNUSED_ID);

  // Nombre d’aiguilles détectées
  Exploration::comptAig(doc["comptAig"]);

  // Masque d’aiguilles
  canton->masqueAig(doc["masqueAig"]);

  // Paramètres généraux
  WIFI_ON = doc["wifi_on"];
  EXPLORATION_ON = doc["exploration_on"];
  ssid_str = doc["ssid"].as<String>();
  password_str = doc["password"].as<String>();
  strcpy(ssid, ssid_str.c_str());
  strcpy(password, password_str.c_str());
  canton->maxSpeed(doc["maxSpeed"]);
  canton->sensMarche(doc["sensMarche"]);

  // Rôle ferroviaire
  if (doc.containsKey("role"))
    canton->setRole((CantonRole)(uint8_t)doc["role"]);
  else
    canton->setRole(ROLE_PLEINE_VOIE);

  // Reconstruction des voisins
  const char *index[] = {"p00", "p01", "p10", "p11", "m00", "m01", "m10", "m11"};
  for (byte i = 0; i < cantonPsize; i++)
  {
    if (doc[index[i]] != "null")
    {
      if (canton->cantonP[i] == nullptr)
        canton->cantonP[i] = new CantonPeriph;
      canton->cantonP[i]->ID(doc[index[i]]);
    }
  }

  // Reconstruction des aiguilles
  for (byte i = 0; i < aigSize; i++)
  {
    if (doc["aig" + String(i)] != "null")
    {
      if (canton->aig[i] == nullptr)
        canton->aig[i] = new Aig;

      canton->aig[i]->ID(doc["aig" + String(i) + "id"]);
      canton->aig[i]->posDroit(doc["aig" + String(i) + "posDroit"]);
      canton->aig[i]->posDevie(doc["aig" + String(i) + "posDevie"]);
      canton->aig[i]->speed(doc["aig" + String(i) + "speed"]);
      canton->aig[i]->pin(doc["aig" + String(i) + "pin"]);

      // 🟩 NOUVEAU : index EXSA côté H / AH
      canton->aig[i]->cantonPdroitIdx(doc["aig" + String(i) + "cantonPdroitIdx"]);
      canton->aig[i]->cantonPdevieIdx(doc["aig" + String(i) + "cantonPdevieIdx"]);

      // 🟦 IMPORTANT : neutralisation du pilotage local
      // canton->aig[i]->setup();  // ❌ Désactivé — EXSA pilote les servos
    }
  }

  // Reconstruction des signaux
  for (byte i = 0; i < signalSize; i++)
  {
    if (doc["sign" + String(i)] != "null")
    {
      if (canton->signal[i] == nullptr)
        canton->signal[i] = new Signal;

      canton->signal[i]->type(doc["sign" + String(i) + "type"]);
      canton->signal[i]->position(doc["sign" + String(i) + "position"]);
    }
  }

  file.close();
}

/*-------------------------------------------------------------
                           writeFile()
   Sauvegarde settings.json
--------------------------------------------------------------*/
void Settings::writeFile()
{
  File file = SPIFFS.open("/settings.json", "w");
  if (!file)
  {
#ifdef DEBUG
    debug.println("Failed to open settings.json\n\n");
#endif
    return;
  }

  DynamicJsonDocument doc(1024);

  doc["idCanton"] = canton->ID();
  doc["comptAig"] = Exploration::comptAig();
  doc["masqueAig"] = canton->masqueAig();
  doc["wifi_on"] = WIFI_ON;
  doc["exploration_on"] = EXPLORATION_ON;
  doc["ssid"] = ssid;
  doc["password"] = password;
  doc["maxSpeed"] = canton->maxSpeed();
  doc["sensMarche"] = canton->sensMarche();

  // Rôle ferroviaire
  doc["role"] = (uint8_t)canton->role();

  // Voisins
  const String index[] = {"p00", "p01", "p10", "p11", "m00", "m01", "m10", "m11"};
  for (byte i = 0; i < cantonPsize; i++)
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
          doc["aig" + String(i)] = "null";
      }
      else
      {
          // 🟩 Correction essentielle : indiquer que l’aiguille existe
          doc["aig" + String(i)] = "Actif";

          doc["aig" + String(i) + "id"] = canton->aig[i]->ID();
          doc["aig" + String(i) + "posDroit"] = canton->aig[i]->posDroit();
          doc["aig" + String(i) + "posDevie"] = canton->aig[i]->posDevie();
          doc["aig" + String(i) + "speed"] = canton->aig[i]->speed();
          doc["aig" + String(i) + "pin"] = canton->aig[i]->pin();

          // 🟩 Nouveaux champs EXSA
          doc["aig" + String(i) + "cantonPdroitIdx"] = canton->aig[i]->cantonPdroitIdx();
          doc["aig" + String(i) + "cantonPdevieIdx"] = canton->aig[i]->cantonPdevieIdx();
      }
  }

  // Signaux
  for (byte i = 0; i < signalSize; i++)
  {
    if (canton->signal[i] == nullptr)
      doc["sign" + String(i)] = "null";
    else
    {
      doc["sign" + String(i) + "type"] = canton->signal[i]->type();
      doc["sign" + String(i) + "position"] = canton->signal[i]->position();
    }
  }

  String output;
  serializeJson(doc, output);
  file.print(output);
  file.close();
}
