/*
 * Node_Role.cpp — Gestion du rôle ferroviaire du canton
 */

#include "Node_Internal.h"
#include "debug_sa.h"

/* ============================================================================
 * setRole()
 * ============================================================================
 */

void Node::setRole(CantonRole role)
{
    m_role = role;

    SA_LOG_INFO("[Node %u] setRole() → rôle ferroviaire = %d\n", m_id, m_role);

    applyRoleDefaults();
}

/* ============================================================================
 * getRole()
 * ============================================================================
 */

CantonRole Node::getRole()
{
    return m_role;
}

/* ============================================================================
 * roleAutoriseAcces() — Version Discovery 2026
 * ============================================================================
 */

bool Node::roleAutoriseAcces(SensDeMarche sens)
{
    switch (m_role)
    {
        case ROLE_BAL:
        case ROLE_PLEINE_VOIE:
            return true;

        case ROLE_ENTREE_GARE:
            return true;

        case ROLE_SORTIE_GARE:
            return true;

        case ROLE_GARE:
            return true;

        case ROLE_MANOEUVRE:
            return false;

        case ROLE_SERVICE:
            return false;

        default:
            return true;
    }
}

/* ============================================================================
 * roleImposeAvertissement()
 * ============================================================================
 */

bool Node::roleImposeAvertissement()
{
    return (m_role == ROLE_ENTREE_GARE);
}

/* ============================================================================
 * roleImposeManoeuvre()
 * ============================================================================
 */

bool Node::roleImposeManoeuvre()
{
    return (m_role == ROLE_MANOEUVRE);
}
