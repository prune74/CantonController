/*
 * SatTopo_UART_FeuxDirection.cpp
 * ------------------------------------------------------------
 * Gestion des feux directionnels envoyés à EXSA :
 *
 *   - E8 : Feu directionnel horaire
 *   - E9 : Feu directionnel anti-horaire
 *
 * Les feux directionnels sont calculés par :
 *      Canton_FeuxDirection.cpp → updateFeuDirection()
 *
 * Ce module ne fait qu’une seule chose :
 *      → transmettre à EXSA les codes calculés (0..4)
 *
 * Aucun calcul métier n’est effectué ici.
 */

#include "SatTopologieUART.h"
#include "Config.h"
#include "Exploration_Protocol.h"
#include "debug_sa.h"

#include "Settings.h" // pour Settings::canton
#include "FeuxDirection.h"
#include "Canton.h"

extern HardwareSerial Serial1;

/*-------------------------------------------------------------
  Envoie les feux de direction depuis l’état courant
  → opcode E8 (horaire) + E9 (anti-horaire)
--------------------------------------------------------------*/
void envoyerFeuxDepuisEtatCourant()
{
  // Mise à jour des feux directionnels dans le Canton
  Settings::canton->updateFeuDirection(SensHoraire);
  Settings::canton->updateFeuDirection(SensAntiHoraire);

  // Lecture du résultat (0..4)
  uint8_t codeH = Settings::canton->getFeuDirection(SensHoraire);
  uint8_t codeAH = Settings::canton->getFeuDirection(SensAntiHoraire);

  // Envoi EXSA
  envoyerFeuDirectionHoraire(codeH);
  envoyerFeuDirectionAntiHoraire(codeAH);

  SA_LOG_INFO("[TopoUART] Feux direction envoyés : H=%u AH=%u\n",
              codeH, codeAH);
}

/*-------------------------------------------------------------
  Envoi du feu directionnel horaire
  → opcode E8
--------------------------------------------------------------*/
void envoyerFeuDirectionHoraire(uint8_t code)
{
  SA_LOG_TRACE("[TopoUART] Feu direction horaire (E8) = %u\n", code);

  Serial1.write(PROTO_SYNC_BYTE);
  Serial1.write(PROTO_E8_DIRECTION_HORAIRE);
  Serial1.write(code);
}

/*-------------------------------------------------------------
  Envoi du feu directionnel anti-horaire
  → opcode E9
--------------------------------------------------------------*/
void envoyerFeuDirectionAntiHoraire(uint8_t code)
{
  SA_LOG_TRACE("[TopoUART] Feu direction anti-horaire (E9) = %u\n", code);

  Serial1.write(PROTO_SYNC_BYTE);
  Serial1.write(PROTO_E9_DIRECTION_ANTIHORAIRE);
  Serial1.write(code);
}
