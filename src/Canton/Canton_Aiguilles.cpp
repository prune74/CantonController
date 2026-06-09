/*
 * Canton_Aiguilles.cpp — Pilotage des aiguilles via EXSA (RS485)
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
 * Objectif Exploration 2026 :
 *   → rendre le pilotage des aiguilles clair, robuste et indépendant.
 */

#include "Canton.h"
#include "Config.h"
#include "debug_sa.h"
#include "SatTopologieUART.h"

/* ============================================================================
 * aigRun() — Pilotage d’une aiguille via EXSA
 * ============================================================================
 */
void Canton::aigRun(byte idx)
{
    // 🔥 STOP global Exploration 2026 : aucune commande d’aiguille autorisée
    if (isStopActive())
    {
        SA_LOG_WARN("[Canton %u] aigRun() BLOQUEE : STOP actif\n", m_id);
        return;
    }

    if (idx >= aigSize)
    {
        SA_LOG_WARN("[Canton %u] aigRun() : index %u hors limites\n", m_id, idx);
        return;
    }

    if (aig[idx] == nullptr)
    {
        SA_LOG_WARN("[Canton %u] aigRun() : aig[%u] non initialisée\n", m_id, idx);
        return;
    }

    /*
     * Détermination de l’adresse EXSA :
     *   - EXSA 0 = côté horaire (SP1)
     *   - EXSA 1 = côté anti‑horaire (SM1)
     */
    uint8_t exsaAdresse = 0;

    if (aig[idx]->cantonPdroitIdx() == m_SP1_idx ||
        aig[idx]->cantonPdevieIdx() == m_SP1_idx)
    {
        exsaAdresse = 0; // côté horaire
    }
    else
    {
        exsaAdresse = 1; // côté anti‑horaire
    }

    SA_LOG_INFO("[Canton %u] aigRun() → EXSA=%u, aiguille=%u\n",
                m_id, exsaAdresse, idx);

    // Envoi de la commande F0 (servoMove)
    envoyerServoMove(exsaAdresse, idx);
}

/* ============================================================================
 * getAiguillePosition() — Lecture de la position physique d’une aiguille
 * ============================================================================
 */
uint8_t Canton::getAiguillePosition(uint8_t idx) const
{
    Aig *a = const_cast<Canton *>(this)->getAig(idx);
    if (!a)
        return 0;

    return a->estDroit() ? 0 : 1;
}
