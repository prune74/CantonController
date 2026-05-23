#include "Node_Internal.h"
#include "debug_sa.h"

void Node::debugTopologieEtAiguilles()
{
    SA_LOG_INFO("==============================================\n");
    SA_LOG_INFO("[Node %u] Diagnostic topologie & aiguilles\n", m_id);
    SA_LOG_INFO("==============================================\n");

    SA_LOG_INFO("SP1_idx = %u\n", m_SP1_idx);
    SA_LOG_INFO("SM1_idx = %u\n", m_SM1_idx);

    SA_LOG_INFO("SP2: acces=%d busy=%d masqueAig=0x%02X\n",
                m_SP2_acces, m_SP2_busy, m_masqueAigSP2);

    SA_LOG_INFO("SM2: acces=%d busy=%d masqueAig=0x%02X\n",
                m_SM2_acces, m_SM2_busy, m_masqueAigSM2);

    SA_LOG_INFO("----------------------------------------------\n");
    SA_LOG_INFO("Aiguilles :\n");

    for (uint8_t i = 0; i < aigSize; i++)
    {
        Aig* a = aig[i];
        if (!a)
            continue;

        SA_LOG_INFO(" - Aig[%u] : droit=%u devie=%u | estDroit=%u\n",
                    i,
                    a->nodePdroitIdx(),
                    a->nodePdevieIdx(),
                    a->estDroit());

        if (a->nodePdroitIdx() == m_SP1_idx || a->nodePdevieIdx() == m_SP1_idx)
            SA_LOG_INFO("     → Côté HORAIRE (SP)\n");
        else if (a->nodePdroitIdx() == m_SM1_idx || a->nodePdevieIdx() == m_SM1_idx)
            SA_LOG_INFO("     → Côté ANTI‑HORAIRE (SM)\n");
        else
            SA_LOG_WARN("     → ⚠️ Aiguille non reliée à SP1 ni SM1 !\n");
    }

    SA_LOG_INFO("----------------------------------------------\n");
    SA_LOG_INFO("Voisins (nodeP[]) :\n");

    for (uint8_t i = 0; i < nodePsize; i++)
    {
        NodePeriph* p = nodeP[i];
        if (!p)
            continue;

        SA_LOG_INFO(" - nodeP[%u] : ID=%u busy=%d acces=%d reserved=%u\n",
                    i,
                    p->ID(),
                    p->busy(),
                    p->acces(),
                    p->reserved());
    }

    SA_LOG_INFO("----------------------------------------------\n");
    SA_LOG_INFO("Signaux :\n");

    if (signal[0])
        SA_LOG_INFO(" - AH : type=%u\n", signal[0]->type());
    else
        SA_LOG_WARN(" - AH : (non initialisé)\n");

    if (signal[1])
        SA_LOG_INFO(" - H  : type=%u\n", signal[1]->type());
    else
        SA_LOG_WARN(" - H  : (non initialisé)\n");

    SA_LOG_INFO("==============================================\n");
}
