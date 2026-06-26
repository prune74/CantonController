/*
 * Canton_Aiguilles.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Pilotage des aiguilles via EXCC (RS485).
 *
 * Rôle :
 *   - envoyer une commande F0 (servoMove) à l’EXCC
 *
 * IMPORTANT 2026 :
 *   - Le CC ne pilote plus les servos localement.
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
#include "CC_CAN_EXCC.h"
#include "Config.h"
#include "debug_cc.h"

/* ============================================================================
 *  aigRun() — Envoi d’une commande F0 à l’EXCC 🟢
 * ==========================================================================*/
void Canton::aigRun(uint8_t idx) // 🟢
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
    // EXCC unique : plus de distinction H/AH, plus d’adresse 0/1
    // On envoie simplement :
    //    - l’index du servo
    //    - la direction logique (0=droit, 1=devie)
    // ------------------------------------------------------------------------
    uint8_t direction = aig[idx]->estDroit() ? 0 : 1;

    CC_LOG_INFO("[Canton %u][Aiguilles][CC] aigRun() → servo=%u direction=%u\n",
                m_id, idx, direction);

    // ------------------------------------------------------------------------
    // Envoi de la commande F0 (servoMove)
    // ------------------------------------------------------------------------
    CC_CAN_EXCC::sendServoMove(this, idx, direction);
}

/* ============================================================================
 *  getAiguillePosition() — Lecture logique de l’aiguille 
 * ==========================================================================*/
uint8_t Canton::getAiguillePosition(uint8_t idx) const // 🟢
{
    Aig *a = const_cast<Canton *>(this)->getAig(idx);
    if (!a)
        return 0;

    return a->estDroit() ? 0 : 1;
}