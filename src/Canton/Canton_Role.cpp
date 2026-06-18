/*
 * Canton_Role.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Gestion du rôle ferroviaire du canton :
 *
 *   ROLE_BAL
 *   ROLE_PLEINE_VOIE
 *   ROLE_ENTREE_GARE
 *   ROLE_SORTIE_GARE
 *   ROLE_GARE
 *   ROLE_MANOEUVRE
 *   ROLE_SERVICE
 *
 * Le rôle influence :
 *   - l’autorisation d’accès (roleAutoriseAcces)
 *   - l’imposition d’un avertissement (roleImposeAvertissement)
 *   - l’imposition d’un mode manœuvre (roleImposeManoeuvre)
 *
 * IMPORTANT :
 *   - aucune logique d’aiguilles ici
 *   - aucune logique de signaux
 *   - aucune logique d’occupation
 *
 * Ce module définit UNIQUEMENT les règles liées au rôle ferroviaire.
 */

#include "Canton.h"
#include "Config.h"
#include "debug_cc.h"

/* ============================================================================
 *  setRole()
 * ---------------------------------------------------------------------------
 *  Définit le rôle ferroviaire du canton et applique les valeurs par défaut.
 * ==========================================================================*/
void Canton::setRole(CantonRole role)
{
    m_role = role;

    CC_LOG_INFO("[Canton %u][Role][CC] setRole → %d\n", m_id, m_role);

    applyRoleDefaults();
}

/* ============================================================================
 *  getRole()
 * ==========================================================================*/
CantonRole Canton::getRole()
{
    return m_role;
}

/* ============================================================================
 *  roleAutoriseAcces() — Règle d’accès selon le rôle
 * ---------------------------------------------------------------------------
 *  ROLE_BAL, ROLE_PLEINE_VOIE, ROLE_ENTREE_GARE, ROLE_SORTIE_GARE, ROLE_GARE
 *      → accès autorisé
 *
 *  ROLE_MANOEUVRE, ROLE_SERVICE
 *      → accès interdit
 *
 *  Par défaut → accès autorisé
 * ==========================================================================*/
bool Canton::roleAutoriseAcces(SensDeMarche sens)
{
    switch (m_role)
    {
        case ROLE_BAL:
        case ROLE_PLEINE_VOIE:
        case ROLE_ENTREE_GARE:
        case ROLE_SORTIE_GARE:
        case ROLE_GARE:
            return true;

        case ROLE_MANOEUVRE:
        case ROLE_SERVICE:
            return false;

        default:
            return true;
    }
}

/* ============================================================================
 *  roleImposeAvertissement()
 * ---------------------------------------------------------------------------
 *  ROLE_ENTREE_GARE → impose un avertissement
 * ==========================================================================*/
bool Canton::roleImposeAvertissement()
{
    return (m_role == ROLE_ENTREE_GARE);
}

/* ============================================================================
 *  roleImposeManoeuvre()
 * ---------------------------------------------------------------------------
 *  ROLE_MANOEUVRE → impose le mode manœuvre
 * ==========================================================================*/
bool Canton::roleImposeManoeuvre()
{
    return (m_role == ROLE_MANOEUVRE);
}
