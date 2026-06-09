/*
 * Canton_Role.cpp — Gestion du rôle ferroviaire du canton
 */

#include "Canton.h"
#include "Config.h"
#include "debug_sa.h"

/* ============================================================================
 * setRole()
 * ============================================================================
 */

void Canton::setRole(CantonRole role)
{
    m_role = role;

    SA_LOG_INFO("[Canton %u] setRole() → rôle ferroviaire = %d\n", m_id, m_role);

    applyRoleDefaults();
}

/* ============================================================================
 * getRole()
 * ============================================================================
 */

CantonRole Canton::getRole()
{
    return m_role;
}

/* ============================================================================
 * roleAutoriseAcces() — Version Exploration 2026
 * ============================================================================
 */

bool Canton::roleAutoriseAcces(SensDeMarche sens)
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

bool Canton::roleImposeAvertissement()
{
    return (m_role == ROLE_ENTREE_GARE);
}

/* ============================================================================
 * roleImposeManoeuvre()
 * ============================================================================
 */

bool Canton::roleImposeManoeuvre()
{
    return (m_role == ROLE_MANOEUVRE);
}
