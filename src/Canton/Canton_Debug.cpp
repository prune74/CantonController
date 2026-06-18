#include "Canton.h"
#include "Config.h"
#include "debug_cc.h"

/*
 * ============================================================================
 *  debugTopologieEtAiguilles()
 * ---------------------------------------------------------------------------
 *  Diagnostic complet de la topologie locale :
 *    - indices SP1 / SM1
 *    - états SP2 / SM2
 *    - aiguilles (géométrie + côté H/AH)
 *    - voisins cantonP[]
 *    - signaux
 *
 *  IMPORTANT 2026 :
 *    - aucun masque d’aiguilles n’est affiché ici
 *    - SP2 / SM2 ne possèdent plus de masque
 *    - ce diagnostic n’effectue AUCUNE logique métier
 * ============================================================================
 */
void Canton::debugTopologieEtAiguilles()
{
    CC_LOG_INFO("============================================================\n");
    CC_LOG_INFO("[Canton %u][Debug][CC] Diagnostic topologie & aiguilles\n", m_id);
    CC_LOG_INFO("============================================================\n");

    /* ------------------------------------------------------------------------
     *  Indices SP1 / SM1
     * ------------------------------------------------------------------------ */
    CC_LOG_INFO("SP1_idx = %u\n", m_SP1_idx);
    CC_LOG_INFO("SM1_idx = %u\n", m_SM1_idx);

    /* ------------------------------------------------------------------------
     *  États SP2 / SM2 (accès secondaires)
     * ------------------------------------------------------------------------ */
    CC_LOG_INFO("SP2 : acces=%d busy=%d\n",
                m_SP2_acces, m_SP2_busy);

    CC_LOG_INFO("SM2 : acces=%d busy=%d\n",
                m_SM2_acces, m_SM2_busy);

    /* ------------------------------------------------------------------------
     *  Aiguilles
     * ------------------------------------------------------------------------ */
    CC_LOG_INFO("------------------------------------------------------------\n");
    CC_LOG_INFO("Aiguilles :\n");

    for (uint8_t i = 0; i < aigSize; i++)
    {
        Aig *a = aig[i];
        if (!a)
            continue;

        CC_LOG_INFO(" - Aig[%u] : droit=%u devie=%u | estDroit=%u\n",
                    i,
                    a->cantonPdroitIdx(),
                    a->cantonPdevieIdx(),
                    a->estDroit());

        if (a->cantonPdroitIdx() == m_SP1_idx || a->cantonPdevieIdx() == m_SP1_idx)
        {
            CC_LOG_INFO("     → Côté HORAIRE (SP)\n");
        }
        else if (a->cantonPdroitIdx() == m_SM1_idx || a->cantonPdevieIdx() == m_SM1_idx)
        {
            CC_LOG_INFO("     → Côté ANTI‑HORAIRE (SM)\n");
        }
        else
        {
            CC_LOG_WARN("     → ⚠️ Aiguille non reliée à SP1 ni SM1 !\n");
        }
    }

    /* ------------------------------------------------------------------------
     *  Voisins cantonP[]
     * ------------------------------------------------------------------------ */
    CC_LOG_INFO("------------------------------------------------------------\n");
    CC_LOG_INFO("Voisins (cantonP[]) :\n");

    for (uint8_t i = 0; i < cantonPsize; i++)
    {
        CantonPeriph *p = cantonP[i];
        if (!p)
            continue;

        CC_LOG_INFO(" - cantonP[%u] : ID=%u busy=%d acces=%d reserved=%u\n",
                    i,
                    p->ID(),
                    p->busy(),
                    p->acces(),
                    p->reserved());
    }

    /* ------------------------------------------------------------------------
     *  Signaux
     * ------------------------------------------------------------------------ */
    CC_LOG_INFO("------------------------------------------------------------\n");
    CC_LOG_INFO("Signaux :\n");

    if (signal[0])
        CC_LOG_INFO(" - AH : type=%u\n", signal[0]->type());
    else
        CC_LOG_WARN(" - AH : (non initialisé)\n");

    if (signal[1])
        CC_LOG_INFO(" - H  : type=%u\n", signal[1]->type());
    else
        CC_LOG_WARN(" - H  : (non initialisé)\n");

    CC_LOG_INFO("============================================================\n");
}
