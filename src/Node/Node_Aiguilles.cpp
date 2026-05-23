/*
 * Node_Aiguilles.cpp — Pilotage des aiguilles via EXSA (RS485)
 * ---------------------------------------------------------------------------
 * Rôle :
 *   Ce module gère toutes les opérations liées aux aiguilles locales du canton.
 *
 *   IMPORTANT 2026 :
 *     - Le SA ne pilote plus les servos localement.
 *     - Toute commande d’aiguille est envoyée à EXSA via RS485.
 *     - EXSA pilote physiquement les servos.
 *
 *   Ce fichier contient :
 *     - aigRun() : envoi d’une commande de mouvement d’aiguille à EXSA
 *
 * Objectif Discovery 2026 :
 *   → rendre le pilotage des aiguilles clair, robuste et indépendant.
 */

#include "Node_Internal.h"

/* ============================================================================
 * aigRun() — Pilotage d’une aiguille via EXSA
 * ---------------------------------------------------------------------------
 * Cette fonction NE pilote PLUS les servos localement.
 *
 * Elle envoie une trame RS485 :
 *      [AA][F0][adresse EXSA][index servo]
 *
 * EXSA reçoit la trame, vérifie si elle lui est destinée,
 * puis pilote le servo correspondant.
 *
 * Mini‑commentaire pédagogique :
 *   - idx = index de l’aiguille dans le tableau aig[]
 *   - nodePdroitIdx() / nodePdevieIdx() permettent de déterminer
 *     si l’aiguille est côté SP1 (horaire) ou SM1 (anti‑horaire).
 * ============================================================================
 */
void Node::aigRun(byte idx)
{
    if (idx >= aigSize)
    {
        SA_LOG_WARN("[Node %u] aigRun() : index %u hors limites\n", m_id, idx);
        return;
    }

    if (aig[idx] == nullptr)
    {
        SA_LOG_WARN("[Node %u] aigRun() : aig[%u] non initialisée\n", m_id, idx);
        return;
    }

    /*
     * Détermination de l’adresse EXSA :
     * ---------------------------------
     * Convention Discovery 2026 :
     *   - EXSA 0 = côté horaire (SP1)
     *   - EXSA 1 = côté anti‑horaire (SM1)
     *
     * On regarde vers quel voisin pointe l’aiguille.
     */
    uint8_t exsaAdresse = 0;

    if (aig[idx]->nodePdroitIdx() == m_SP1_idx ||
        aig[idx]->nodePdevieIdx() == m_SP1_idx)
    {
        exsaAdresse = 0;   // côté horaire
    }
    else
    {
        exsaAdresse = 1;   // côté anti‑horaire
    }

    SA_LOG_INFO("[Node %u] aigRun() → EXSA=%u, aiguille=%u\n",
                m_id, exsaAdresse, idx);

    /*
     * Envoi de la commande F0 (servoMove)
     * -----------------------------------
     * envoyerServoMove() est fourni par SatTopologieUART.
     */
    envoyerServoMove(exsaAdresse, idx);
}

/* ============================================================================
 * getAiguillePosition() — Lecture de la position physique d’une aiguille
 * ---------------------------------------------------------------------------
 * Cette fonction retourne :
 *   - 0 si l’aiguille est en position DROITE
 *   - 1 si l’aiguille est en position DEVIÉE
 *
 * Elle est utilisée par le module FeuxDirection pour déterminer
 * la voie réellement ouverte.
 * ============================================================================
 */
uint8_t Node::getAiguillePosition(uint8_t idx) const
{
    Aig* a = const_cast<Node*>(this)->getAig(idx);
    if (!a)
        return 0;

    return a->estDroit() ? 0 : 1;
}

/* ============================================================================
 * fin de src/Node/Node_Aiguilles.cpp
 * ============================================================================
 */