/*
 * Node_Stop.cpp — Gestion du STOP global Discovery 2026
 */

#include "Node_Internal.h"
#include "SatEXSA_Link.h"
#include "debug_sa.h"

void Node::setStopActive(bool v)
{
    if (m_stopActive == v)
        return; // aucun changement

    m_stopActive = v;

    // Quel EXSA possède réellement le booster ?
    int8_t exsaBooster = SatEXSA_Link::getBoosterExsaIndex();

    if (exsaBooster < 0)
    {
        SA_LOG_ERROR("[Node %u] STOP demandé mais aucun EXSA n’a déclaré de booster !\n", m_id);
        return;
    }

    if (m_stopActive)
    {
        SA_LOG_ERROR("[Node %u] STOP global ACTIVÉ → Booster EXSA %d OFF\n",
                     m_id, exsaBooster);

        SatEXSA_Link::envoyerBoosterPower(exsaBooster, false);
    }
    else
    {
        SA_LOG_INFO("[Node %u] STOP global LEVÉ → Booster EXSA %d ON\n",
                    m_id, exsaBooster);

        SatEXSA_Link::envoyerBoosterPower(exsaBooster, true);
    }
}
