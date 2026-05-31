/*
 * SatTopo_UART_Signaux.cpp
 * ------------------------------------------------------------
 * Gestion des signaux ferroviaires envoyés à EXSA :
 *
 *   - E5 : Configuration des signaux (type + position)
 *   - E6 : Aspect SNCF horaire
 *   - E7 : Aspect SNCF anti-horaire
 *
 * Ce module ne contient aucune logique métier :
 * il transmet simplement à EXSA ce que Settings_JSON et
 * SupervisionCanton ont déterminé.
 */

#include "SatTopologieUART.h"
#include "Config.h"
#include "Discovery_Protocol.h"
#include "debug_sa.h"

#include <SPIFFS.h>
#include <FS.h>
#include <ArduinoJson.h>

extern HardwareSerial Serial1;

/*-------------------------------------------------------------
  Envoie la configuration des signaux depuis settings.json
  → opcode E5
--------------------------------------------------------------*/
void envoyerConfigurationSignauxDepuisSettings()
{
  SA_LOG_INFO("[TopoUART] Envoi configuration signaux (E5)...\n");

  if (!SPIFFS.begin(true))
  {
    SA_LOG_ERROR("[TopoUART] SPIFFS indisponible\n");
    return;
  }

  File file = SPIFFS.open("/settings.json", "r");
  if (!file)
  {
    SA_LOG_ERROR("[TopoUART] settings.json introuvable\n");
    return;
  }

  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error)
  {
    SA_LOG_ERROR("[TopoUART] Erreur JSON : %s\n", error.c_str());
    return;
  }

  uint8_t type0 = doc["sign0type"] | 0;
  uint8_t type1 = doc["sign1type"] | 0;
  uint8_t pos0 = doc["sign0position"] | 0;
  uint8_t pos1 = doc["sign1position"] | 0;

  SA_LOG_TRACE("[TopoUART] signaux : T0=%u T1=%u P0=%u P1=%u\n",
               type0, type1, pos0, pos1);

  Serial1.write(PROTO_SYNC_BYTE);
  Serial1.write(PROTO_E5_CONFIG_SIGNAUX);
  Serial1.write(type0);
  Serial1.write(type1);
  Serial1.write(pos0);
  Serial1.write(pos1);

  SA_LOG_INFO("[TopoUART] Configuration signaux envoyée (E5)\n");
}

/*-------------------------------------------------------------
  Envoi des aspects SNCF (1 octet = enum ExsaAspect)
  → opcode E6 (horaire)
--------------------------------------------------------------*/
void envoyerAspectSignalHoraire(uint8_t aspect)
{
  SA_LOG_TRACE("[TopoUART] Aspect horaire (E6) = %u\n", aspect);

  Serial1.write(PROTO_SYNC_BYTE);
  Serial1.write(PROTO_E6_ASPECT_HORAIRE);
  Serial1.write(aspect);
}

/*-------------------------------------------------------------
  Envoi des aspects SNCF (1 octet = enum ExsaAspect)
  → opcode E7 (anti-horaire)
--------------------------------------------------------------*/
void envoyerAspectSignalAntiHoraire(uint8_t aspect)
{
  SA_LOG_TRACE("[TopoUART] Aspect anti-horaire (E7) = %u\n", aspect);

  Serial1.write(PROTO_SYNC_BYTE);
  Serial1.write(PROTO_E7_ASPECT_ANTIHORAIRE);
  Serial1.write(aspect);
}
