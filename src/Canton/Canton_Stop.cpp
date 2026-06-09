/*
 * Canton_Stop.cpp — Gestion du STOP global Exploration 2026
 */

#include "Canton.h"
#include "Config.h"
#include "SatEXSA_Link.h"
#include "debug_sa.h"

void Canton::setStopActive(bool v)
{
    if (m_stopActive == v)
        return; // aucun changement

    m_stopActive = v;

    // Quel EXSA possède réellement le booster ?
    int8_t exsaBooster = SatEXSA_Link::getBoosterExsaIndex();

    if (exsaBooster < 0)
    {
        SA_LOG_ERROR("[Canton %u] STOP demandé mais aucun EXSA n’a déclaré de booster !\n", m_id);
        return;
    }

    if (m_stopActive)
    {
        SA_LOG_ERROR("[Canton %u] STOP global ACTIVÉ → Booster EXSA %d OFF\n",
                     m_id, exsaBooster);

        SatEXSA_Link::envoyerBoosterPower(exsaBooster, false);
    }
    else
    {
        SA_LOG_INFO("[Canton %u] STOP global LEVÉ → Booster EXSA %d ON\n",
                    m_id, exsaBooster);

        SatEXSA_Link::envoyerBoosterPower(exsaBooster, true);
    }
}
