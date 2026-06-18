/*
 * SatTopo_UART_Aiguilles.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Envoi des mouvements réels d’aiguilles vers l’EXCC via UART (commande F0).
 *
 * IMPORTANT :
 *   - EXCC est unique → plus de côté H/AH
 *   - aucune adresse 0/1 n’est envoyée
 *   - on envoie seulement : servoIndex + direction logique
 */

#include "SatTopologieUART.h"
#include "Config.h"
#include "Exploration_Protocol.h"
#include "debug_cc.h"

#include "Settings.h"
#include "Aig.h"
#include "Canton.h"

extern HardwareSerial Serial1;

/* ============================================================================
 *  envoyerAiguillesDepuisEtatCourant()
 * ---------------------------------------------------------------------------
 *  Envoie F0 pour chaque aiguille logique active.
 *  F0 = mouvement réel → EXCC choisit posDroit() ou posDevie().
 * ==========================================================================*/
void envoyerAiguillesDepuisEtatCourant()
{
    for (uint8_t idx = 0; idx < aigSize; ++idx)
    {
        Aig *aig = Settings::canton->getAig(idx);
        if (!aig)
            continue;

        // -------------------------------------------------------------------
        // EXCC unique : direction = état logique de l’aiguille
        // -------------------------------------------------------------------
        uint8_t direction = aig->estDroit() ? 0 : 1;

        // -------------------------------------------------------------------
        // F0 = mouvement réel
        // -------------------------------------------------------------------
        envoyerServoMove(idx, direction);

        CC_LOG_INFO("[TopoUART][CC] F0 → servo=%u direction=%u (estDroit=%u)\n",
                    idx, direction, aig->estDroit());
    }

    CC_LOG_INFO("[TopoUART][CC] Aiguilles renvoyées (F0) après reboot EXCC\n");
}
