/*
 * Canton_Aiguilles.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Pilotage des aiguilles via EXCC (RS485).
 *
 * Rôle :
 *   - envoyer une commande F0 (servoMove) à l’EXCC concerné
 *   - déterminer automatiquement si l’aiguille appartient au côté H ou AH
 *
 * IMPORTANT 2026 :
 *   - Le SA ne pilote plus les servos localement.
 *   - Toute commande d’aiguille est envoyée à EXCC via RS485.
 *   - EXCC pilote physiquement les servos.
 *
 * Ce module NE contient AUCUNE logique métier :
 *   - ne décide pas droit/dévié
 *   - ne lit pas les positions servo
 *   - ne calcule pas la topologie
 *
 * Il se contente d’envoyer :
 *      “Bouge l’aiguille X selon son état logique actuel”
 */

#include "Canton.h"
#include "Config.h"
#include "debug_cc.h"
#include "SatTopologieUART.h"

/* ============================================================================
 *  aigRun() — Envoi d’une commande F0 à l’EXCC
 * ==========================================================================*/
void Canton::aigRun(byte idx)
{
    // ------------------------------------------------------------------------
    // STOP global Exploration 2026 : aucune commande d’aiguille autorisée
    // ------------------------------------------------------------------------
    if (isStopActive())
    {
        CC_LOG_WARN("[Canton %u][Aiguilles][CC] aigRun() BLOQUEE : STOP actif\n", m_id);
        return;
    }

    // ------------------------------------------------------------------------
    // Vérifications de base
    // ------------------------------------------------------------------------
    if (idx >= aigSize)
    {
        CC_LOG_WARN("[Canton %u][Aiguilles][CC] aigRun() : index %u hors limites\n",
                    m_id, idx);
        return;
    }

    if (aig[idx] == nullptr)
    {
        CC_LOG_WARN("[Canton %u][Aiguilles][CC] aigRun() : aig[%u] non initialisée\n",
                    m_id, idx);
        return;
    }

    // ------------------------------------------------------------------------
    // Détermination de l’adresse EXCC :
    //   - EXCC 0 = côté horaire (SP1)
    //   - EXCC 1 = côté anti‑horaire (SM1)
    // ------------------------------------------------------------------------
    uint8_t exccAdresse = 0;

    if (aig[idx]->cantonPdroitIdx() == m_SP1_idx ||
        aig[idx]->cantonPdevieIdx() == m_SP1_idx)
    {
        exccAdresse = 0; // côté horaire
    }
    else
    {
        exccAdresse = 1; // côté anti‑horaire
    }

    CC_LOG_INFO("[Canton %u][Aiguilles][CC] aigRun() → EXCC=%u, aiguille=%u\n",
                m_id, exccAdresse, idx);

    // ------------------------------------------------------------------------
    // Envoi de la commande F0 (servoMove)
    // ------------------------------------------------------------------------
    envoyerServoMove(exccAdresse, idx);
}

/* ============================================================================
 *  getAiguillePosition() — Lecture logique de l’aiguille
 * ---------------------------------------------------------------------------
 *  Retourne :
 *    - 0 si l’aiguille est en position DROITE
 *    - 1 si l’aiguille est en position DEVIÉE
 *
 *  NOTE :
 *    Ce n’est PAS la position physique du servo.
 *    C’est la position logique stockée dans Aig.cpp.
 * ==========================================================================*/
uint8_t Canton::getAiguillePosition(uint8_t idx) const
{
    Aig *a = const_cast<Canton *>(this)->getAig(idx);
    if (!a)
        return 0;

    return a->estDroit() ? 0 : 1;
}
