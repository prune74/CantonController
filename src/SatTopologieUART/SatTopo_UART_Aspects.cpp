/*
 * SatTopo_UART_Aspects.cpp
 * ------------------------------------------------------------
 * Gestion des aspects SNCF envoyés à EXSA :
 *
 *   - E6 : Aspect horaire
 *   - E7 : Aspect anti-horaire
 *
 * Le calcul des aspects est effectué dans :
 *      SupervisionCanton.cpp → mettreAJourAspectCanton()
 *
 * Ce module ne fait qu’une seule chose :
 *      → transmettre à EXSA les aspects calculés (enum ExsaAspect)
 *
 * Aucun calcul métier n’est effectué ici.
 */

#include "SatTopologieUART.h"
#include "Config.h"
#include "Discovery_Protocol.h"
#include "debug_sa.h"

#include "Settings.h"
#include "SupervisionCanton.h"

extern HardwareSerial Serial1;

/*-------------------------------------------------------------
  Envoie les aspects SNCF depuis l’état courant
  → opcode E6 (horaire) + E7 (anti-horaire)
--------------------------------------------------------------*/
void envoyerAspectsDepuisEtatCourant()
{
  ExsaAspect aspectHoraire =
      mettreAJourAspectCanton(Settings::node, 0);

  ExsaAspect aspectAntiHoraire =
      mettreAJourAspectCanton(Settings::node, 1);

  envoyerAspectSignalHoraire(aspectHoraire);
  envoyerAspectSignalAntiHoraire(aspectAntiHoraire);

  SA_LOG_INFO("[TopoUART] Aspects envoyés : H=%u AH=%u\n",
              aspectHoraire, aspectAntiHoraire);
}
