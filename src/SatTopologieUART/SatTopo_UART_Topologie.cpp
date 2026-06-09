/*
 * SatTopo_UART_Topologie.cpp
 * ------------------------------------------------------------
 * Gestion de la topologie CAN (voisins SP/SM) envoyée à EXSA
 * via l’opcode E4.
 *
 * Rôle :
 *  - Lire settings.json
 *  - Construire les listes de précédents / suivants
 *  - Envoyer la trame E4 à EXSA
 *  - Déclencher l’envoi une seule fois lorsque la topologie est prête
 *
 * Ce module ne contient AUCUNE logique métier :
 * il se contente de transmettre à EXSA ce que settings.json contient.
 */

#include "SatTopologieUART.h"
#include "Config.h"
#include "Exploration_Protocol.h"
#include "debug_sa.h"

#include <SPIFFS.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <vector>

extern HardwareSerial Serial1;

/*-------------------------------------------------------------
  Vérifie si au moins un voisin est connu dans settings.json
  (fonction interne, non exposée dans le header)
--------------------------------------------------------------*/
static bool tousLesVoisinsSontConnus()
{
  SA_LOG_TRACE("[TopoUART] Vérification des voisins dans settings.json\n");

  if (!SPIFFS.begin(true))
  {
    SA_LOG_ERROR("[TopoUART] SPIFFS indisponible\n");
    return false;
  }

  File file = SPIFFS.open("/settings.json", "r");
  if (!file)
  {
    SA_LOG_WARN("[TopoUART] settings.json introuvable\n");
    return false;
  }

  StaticJsonDocument<4096> doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error)
  {
    SA_LOG_ERROR("[TopoUART] Erreur JSON : %s\n", error.c_str());
    return false;
  }

  uint8_t compteur = 0;
  for (const char *cle : {"p00", "p01", "p10", "p11",
                          "m00", "m01", "m10", "m11"})
  {
    if (doc[cle].is<uint8_t>() && doc[cle] != UNUSED_ID)
      compteur++;
  }

  SA_LOG_INFO("[TopoUART] %u voisins connus\n", compteur);

  return compteur >= 1;
}

/*-------------------------------------------------------------
  Envoie la topologie (voisins SP/SM) à partir de settings.json
  → opcode E4
--------------------------------------------------------------*/
void envoyerTopologieDepuisSettings()
{
  SA_LOG_INFO("[TopoUART] Envoi de la topologie EXSA...\n");

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

  StaticJsonDocument<4095> doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error)
  {
    SA_LOG_ERROR("[TopoUART] Erreur JSON : %s\n", error.c_str());
    return;
  }

  uint8_t idLocal = doc["idCanton"] | UNUSED_ID;

  std::vector<uint8_t> precedents;
  for (const char *cle : {"m00", "m01", "m10", "m11"})
  {
    if (doc[cle].is<uint8_t>())
    {
      uint8_t id = doc[cle];
      if (id != UNUSED_ID)
        precedents.push_back(id);
    }
  }

  std::vector<uint8_t> suivants;
  for (const char *cle : {"p00", "p01", "p10", "p11"})
  {
    if (doc[cle].is<uint8_t>())
    {
      uint8_t id = doc[cle];
      if (id != UNUSED_ID)
        suivants.push_back(id);
    }
  }

  SA_LOG_TRACE("[TopoUART] idLocal=%u, precedents=%u, suivants=%u\n",
               idLocal, precedents.size(), suivants.size());

  Serial1.write(PROTO_SYNC_BYTE);
  Serial1.write(PROTO_E4_TOPOLOGIE_CAN);
  Serial1.write(idLocal);

  Serial1.write(precedents.size());
  for (uint8_t id : precedents)
    Serial1.write(id);

  Serial1.write(suivants.size());
  for (uint8_t id : suivants)
    Serial1.write(id);

  SA_LOG_INFO("[TopoUART] Topologie envoyée à EXSA\n");
}

/*-------------------------------------------------------------
  Envoi conditionnel de la topologie (une seule fois)
  - Vérifie si au moins un voisin est connu
  - Envoie E4 (topologie) + E5 (config signaux)
--------------------------------------------------------------*/
void envoyerTopologieSiPret()
{
  static bool dejaEnvoyee = false;

  if (dejaEnvoyee)
    return;

  if (tousLesVoisinsSontConnus())
  {
    SA_LOG_INFO("[TopoUART] Topologie prête → envoi\n");

    envoyerTopologieDepuisSettings();
    envoyerConfigurationSignauxDepuisSettings();

    dejaEnvoyee = true;
  }
  else
  {
    SA_LOG_TRACE("[TopoUART] Topologie non prête\n");
  }
}
