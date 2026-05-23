/*
 * Node_Signaux.cpp — Gestion des signaux H / AH du canton
 */

#include "Node_Internal.h"
#include "debug_sa.h"

/* ============================================================================
 * applyRoleDefaults() — Application automatique des types de signaux
 * ============================================================================
 */
void Node::applyRoleDefaults()
{
    if (signal[0] == nullptr || signal[1] == nullptr)
    {
        SA_LOG_WARN("[Node %u] applyRoleDefaults() ignoré : signaux non initialisés\n", m_id);
        return;
    }

    uint8_t typeH  = signal[1]->type();
    uint8_t typeAH = signal[0]->type();

    SA_LOG_TRACE("[Node %u] applyRoleDefaults() rôle=%d | H=%d AH=%d\n",
                 m_id, m_role, typeH, typeAH);

    switch (m_role)
    {
        case ROLE_BAL:
            if (typeH  == SIG_SIMPLE) signal[1]->type(SIG_BAL);
            if (typeAH == SIG_SIMPLE) signal[0]->type(SIG_BAL);
            break;

        case ROLE_ENTREE_GARE:
            if (typeH  == SIG_SIMPLE) signal[1]->type(SIG_ENTREE);
            if (typeAH == SIG_SIMPLE) signal[0]->type(SIG_ENTREE);
            break;

        case ROLE_SORTIE_GARE:
            if (typeH  == SIG_SIMPLE) signal[1]->type(SIG_SORTIE);
            if (typeAH == SIG_SIMPLE) signal[0]->type(SIG_SORTIE);
            break;

        case ROLE_MANOEUVRE:
            if (typeH  == SIG_SIMPLE) signal[1]->type(SIG_MANOEUVRE);
            if (typeAH == SIG_SIMPLE) signal[0]->type(SIG_MANOEUVRE);
            break;

        case ROLE_GARE:
        case ROLE_SERVICE:
        case ROLE_PLEINE_VOIE:
        default:
            // Signaux simples → pas de modification
            break;
    }

    SA_LOG_INFO("[Node %u] Types de signaux mis à jour selon le rôle\n", m_id);
}
