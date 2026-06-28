/*
 * Canton_Stop.cpp — Gestion du STOP global
 * ---------------------------------------------------------------------------
 * Ce module gère l’activation / désactivation du STOP global pour le CC.
 *
 * Rôle :
 *   - activer ou lever le STOP local
 *   - couper ou réactiver le booster via l’EXCC
 *   - suspendre ou reprendre le heartbeat CC → ERM
 *
 * Important :
 *   - aucune logique métier ici
 *   - aucune logique d’aiguilles ou de signaux
 *   - ce module relaie simplement l’ordre STOP → EXCC
 *   - la suspension du heartbeat permet au ERM de déclencher STOP global
 */

#include "Canton.h"
#include "Config.h"
#include "EXCC_Link.h"
#include "CCWatchdog.h"
#include "debug_cc.h"

/* ============================================================================
 *  setStopActive() — Activation / désactivation du STOP local
 * ---------------------------------------------------------------------------
 *  v = true  → STOP activé  → Booster OFF  → Heartbeat OFF
 *  v = false → STOP levé    → Booster ON   → Heartbeat ON
 *
 *  Architecture :
 *      - un seul EXCC → pas d’index à gérer
 *      - le CC ne doit PAS envoyer de STOP global (0x201)
 *      - le ERM détecte l’absence de heartbeat → STOP global
 * ==========================================================================*/
void Canton::setStopActive(bool v)
{
    if (m_stopActive == v)
        return; // aucun changement

    m_stopActive = v;

    if (m_stopActive)
    {
        // --------------------------------------------------------------------
        // STOP ACTIVÉ
        // --------------------------------------------------------------------
        CC_LOG_ERROR("[Canton %u][STOP] STOP ACTIVÉ → Booster OFF\n", m_id);

        // 1) Couper le booster local
        EXCC_Link::envoyerBoosterPower(false);

        // 2) Suspendre le heartbeat → ERM déclenche STOP global
        CCWatchdog_suspend();
    }
    else
    {
        // --------------------------------------------------------------------
        // STOP LEVÉ
        // --------------------------------------------------------------------
        CC_LOG_INFO("[Canton %u][STOP] STOP LEVÉ → Booster ON\n", m_id);

        // 1) Réactiver le booster
        EXCC_Link::envoyerBoosterPower(true);

        // 2) Reprendre le heartbeat
        CCWatchdog_resume();
    }
}
