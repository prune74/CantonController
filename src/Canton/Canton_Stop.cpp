/*
 * Canton_Stop.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Gestion du STOP global Exploration 2026.
 *
 * Rôle :
 *   - activer / désactiver le STOP global
 *   - demander à l’EXCC possédant le booster de couper / réactiver la voie
 *
 * IMPORTANT :
 *   - aucune logique métier ici
 *   - aucune logique d’aiguilles ou de signaux
 *   - ce module ne fait que relayer l’ordre STOP → EXCC
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
 *  Le booster réel est déterminé par :
 *      EXCC_Link::getBoosterExccIndex()
 *
 *  Si aucun EXCC ne déclare de booster → erreur critique.
 * ==========================================================================*/
void Canton::setStopActive(bool v)
{
    if (m_stopActive == v)
        return; // aucun changement

    m_stopActive = v;

    // Quel EXCC possède réellement le booster ?
    int8_t exccBooster = EXCC_Link::getBoosterExccIndex();

    if (exccBooster < 0)
    {
        CC_LOG_ERROR("[Canton %u][STOP][CC] STOP demandé mais aucun EXCC n’a déclaré de booster !\n",
                     m_id);
        return;
    }

    if (m_stopActive)
    {
        CC_LOG_ERROR("[Canton %u][STOP][CC] STOP ACTIVÉ → Booster EXCC %d OFF\n",
                     m_id, exccBooster);

        EXCC_Link::envoyerBoosterPower(exccBooster, false);
    }
    else
    {
        CC_LOG_INFO("[Canton %u][STOP][CC] STOP LEVÉ → Booster EXCC %d ON\n",
                    m_id, exccBooster);

        EXCC_Link::envoyerBoosterPower(exccBooster, true);
    }
}
