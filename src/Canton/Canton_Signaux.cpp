/*
 * Canton_Signaux.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * IMPORTANT 2026 :
 *   - Les rôles ferroviaires sont supprimés.
 *   - Aucun type de signal n'est imposé automatiquement.
 *   - applyRoleDefaults() est conservée uniquement pour compatibilité,
 *     mais ne fait plus rien.
 *
 * Ce module ne contient plus aucune logique métier.
 */

#include "Canton.h"
#include "Config.h"
#include "debug_cc.h"

/* ============================================================================
 *  applyRoleDefaults() — Désactivé
 * ---------------------------------------------------------------------------
 *  Avant 2026 :
 *    - modifiait automatiquement les types de signaux selon le rôle
 *
 *  En 2026 :
 *    - les rôles sont supprimés
 *    - les types de signaux sont définis uniquement par :
 *        → la configuration JSON
 *        → la logique topologique (à venir)
 *
 *  Cette fonction est conservée pour compatibilité mais ne fait rien.
 * ==========================================================================*/
void Canton::applyRoleDefaults()
{
    CC_LOG_INFO("[Canton %u][Signaux][CC] applyRoleDefaults désactivé (rôles supprimés)\n",
                m_id);
}
