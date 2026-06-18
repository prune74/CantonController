/*
 * Canton_Stop.cpp — Gestion du STOP global
 * ---------------------------------------------------------------------------
 * Ce module gère l’activation / désactivation du STOP global.
 *
 * Rôle :
 *   - activer ou lever le STOP
 *   - demander à l’unique EXCC de couper ou réactiver le booster
 *
 * Important :
 *   - aucune logique métier ici
 *   - aucune logique d’aiguilles ou de signaux
 *   - ce module relaie simplement l’ordre STOP → EXCC
 */

#include "Canton.h"
#include "Config.h"
#include "EXCC_Link.h"
#include "debug_cc.h"

/* ============================================================================
 *  setStopActive() — Activation / désactivation du STOP global
 * ---------------------------------------------------------------------------
 *  v = true  → STOP activé  → Booster OFF
 *  v = false → STOP levé    → Booster ON
 *
 *  Architecture :
 *      Il n’existe qu’un seul EXCC → aucun index à gérer.
 * ==========================================================================*/
void Canton::setStopActive(bool v)
{
    if (m_stopActive == v)
        return; // aucun changement

    m_stopActive = v;

    if (m_stopActive)
    {
        CC_LOG_ERROR("[Canton %u][STOP] STOP ACTIVÉ → Booster OFF\n", m_id);
        EXCC_Link::envoyerBoosterPower(false);
    }
    else
    {
        CC_LOG_INFO("[Canton %u][STOP] STOP LEVÉ → Booster ON\n", m_id);
        EXCC_Link::envoyerBoosterPower(true);
    }
}
