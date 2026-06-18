/*
 * SatTopo_UART_Aiguilles.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Envoi des mouvements réels d’aiguilles vers les EXCC via UART (commande F0).
 *
 * Rôle :
 *   - pour chaque aiguille logique (0..5)
 *       • déterminer quel EXCC la pilote (côté H ou AH)
 *       • envoyer F0 pour déclencher le mouvement réel
 *
 * IMPORTANT :
 *   - ce module ne décide PAS de la position (droit/dévié)
 *   - ce module ne lit PAS les positions servo
 *   - ce module ne calcule PAS la topologie
 *
 * Toute la logique métier (estDroit(), posDroit(), posDevie(), etc.)
 * est gérée dans Aig.cpp et Canton_Aiguilles.cpp.
 *
 * Ici, on se contente d’envoyer :
 *      “Bouge l’aiguille X selon son état logique actuel”
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
    for (uint8_t idx = 0; idx < 6; ++idx)
    {
        Aig *aig = Settings::canton->getAig(idx);
        if (!aig)
            continue;

        // -------------------------------------------------------------------
        // Déterminer quel EXCC pilote cette aiguille
        // -------------------------------------------------------------------
        uint8_t exccAdresse =
            (aig->cantonPdroitIdx() == Settings::canton->SP1_idx()) ? 0 : 1;

        // -------------------------------------------------------------------
        // F0 = mouvement réel
        // -------------------------------------------------------------------
        envoyerServoMove(exccAdresse, idx);

        CC_LOG_INFO("[TopoUART][CC] F0 → EXCC %u, aiguille %u (estDroit=%u)\n",
                    exccAdresse, idx, aig->estDroit());
    }

    CC_LOG_INFO("[TopoUART][CC] Aiguilles renvoyées (F0) après reboot EXCC\n");
}
