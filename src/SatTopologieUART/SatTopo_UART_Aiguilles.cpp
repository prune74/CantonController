/*
 * SatTopo_UART_Aiguilles.cpp
 * ------------------------------------------------------------
 * Gestion des aiguilles logiques envoyées à EXSA via F0.
 *
 *  - F0 : Mouvement réel d’une aiguille
 *
 * Le rôle de ce module est extrêmement simple :
 *    → Pour chaque aiguille logique (0..5)
 *         - déterminer quel EXSA la pilote (H ou AH)
 *         - envoyer F0 pour demander le mouvement réel
 *
 * IMPORTANT :
 *   - Ce module ne décide PAS de la position (droit/dévié)
 *   - Ce module ne lit PAS les positions servo
 *   - Ce module ne calcule PAS la topologie
 *
 * Il se contente d’envoyer à EXSA :
 *      “Bouge l’aiguille X selon l’état logique actuel”
 *
 * La logique métier (estDroit(), posDroit(), posDevie(), etc.)
 * est entièrement gérée dans Aig.cpp et Canton_Aiguilles.cpp.
 */

#include "SatTopologieUART.h"
#include "Config.h"
#include "Exploration_Protocol.h"
#include "debug_sa.h"

#include "Settings.h"
#include "Aig.h"
#include "Canton.h"

extern HardwareSerial Serial1;

/*-------------------------------------------------------------
  Envoie l’état logique des aiguilles (F0) depuis l’état courant
  --------------------------------------------------------------
  Pour chaque aiguille logique :
    - on lit estDroit()
    - on détermine quel EXSA la pilote (H ou AH)
    - on envoie F0 pour demander le mouvement réel
--------------------------------------------------------------*/
void envoyerAiguillesDepuisEtatCourant()
{
  for (uint8_t idx = 0; idx < 6; ++idx)
  {
    Aig *aig = Settings::canton->getAig(idx);
    if (!aig)
      continue;

    // Déterminer quel EXSA pilote cette aiguille
    uint8_t exsaAdresse =
        (aig->cantonPdroitIdx() == Settings::canton->SP1_idx()) ? 0 : 1;

    // F0 = mouvement réel → EXSA choisit posDroit ou posDevie
    envoyerServoMove(exsaAdresse, idx);

    SA_LOG_INFO("[TopoUART] F0 → EXSA %u, aiguille %u (estDroit=%u)\n",
                exsaAdresse, idx, aig->estDroit());
  }

  SA_LOG_INFO("[TopoUART] Aiguilles renvoyées (F0) après reboot EXSA\n");
}
