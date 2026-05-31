/*
 * SatTopo_UART_Occupation.cpp
 * ------------------------------------------------------------
 * Gestion de l’occupation des cantons voisins envoyée à EXSA :
 *
 *   - EA : Occupation SP1 / SM1
 *
 * Le SA lit l’état d’occupation des deux voisins directs :
 *      - SP1 (sens horaire)
 *      - SM1 (sens anti-horaire)
 *
 * Ces informations proviennent de NodePeriph et reflètent
 * l’état réel du réseau (capteurs, essieux, supervision).
 *
 * Ce module ne fait qu’une seule chose :
 *      → transmettre à EXSA un octet compact (2 bits)
 *
 *      bit 1 = SP1 occupé
 *      bit 0 = SM1 occupé
 *
 * Aucun calcul métier n’est effectué ici.
 */

#include "SatTopologieUART.h"
#include "Config.h"
#include "Discovery_Protocol.h"
#include "debug_sa.h"

#include "Settings.h"
#include "Node.h"

extern HardwareSerial Serial1;

/*-------------------------------------------------------------
  Envoie l’occupation SP1 / SM1 depuis l’état courant
  → opcode EA
--------------------------------------------------------------*/
void envoyerOccupationDepuisEtatCourant()
{
  uint8_t occSP1 = 0;
  uint8_t occSM1 = 0;

  NodePeriph *sp1 = Settings::node->getNodeP(Settings::node->SP1_idx());
  NodePeriph *sm1 = Settings::node->getNodeP(Settings::node->SM1_idx());

  if (sp1 && sp1->busy())
    occSP1 = 1;
  if (sm1 && sm1->busy())
    occSM1 = 1;

  uint8_t valeur = (occSP1 << 1) | occSM1;

  envoyerOccupationVoisins(valeur);

  SA_LOG_INFO("[TopoUART] EA envoyé : SP1=%u SM1=%u (valeur=%u)\n",
              occSP1, occSM1, valeur);
}

/*-------------------------------------------------------------
  Envoi brut de l’occupation (2 bits)
  → opcode EA
--------------------------------------------------------------*/
void envoyerOccupationVoisins(uint8_t valeur)
{
  SA_LOG_TRACE("[TopoUART] Occupation voisins (EA) = %u\n", valeur);

  Serial1.write(PROTO_SYNC_BYTE);
  Serial1.write(PROTO_EA_OCCUPATION_VOISINS);
  Serial1.write(valeur);
}
