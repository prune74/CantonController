/*
 * Canton_Signaux.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Gestion des types de signaux H / AH selon le rôle ferroviaire du canton.
 *
 * Rôle :
 *   - appliquer automatiquement les types de signaux SNCF
 *     en fonction du rôle (BAL, entrée, sortie, manœuvre…)
 *
 * IMPORTANT :
 *   - aucune logique d’aspect ici
 *   - aucune logique métier
 *   - aucune décision d’ouverture de voie
 *
 * Ce module définit UNIQUEMENT les types de signaux (SIG_BAL, SIG_ENTREE…)
 * en fonction du rôle ferroviaire.
 */

#include "Canton.h"
#include "Config.h"
#include "debug_cc.h"

/* ============================================================================
 *  applyRoleDefaults() — Application automatique des types de signaux
 * ---------------------------------------------------------------------------
 *  Si un signal est encore en SIG_SIMPLE, on le remplace par le type
 *  correspondant au rôle :
 *
 *    ROLE_BAL          → SIG_BAL
 *    ROLE_ENTREE_GARE  → SIG_ENTREE
 *    ROLE_SORTIE_GARE  → SIG_SORTIE
 *    ROLE_MANOEUVRE    → SIG_MANOEUVRE
 *
 *  Les autres rôles ne modifient pas les signaux.
 * ==========================================================================*/
void Canton::applyRoleDefaults()
{
    if (signal[0] == nullptr || signal[1] == nullptr)
    {
        CC_LOG_WARN("[Canton %u][Signaux][CC] applyRoleDefaults ignoré : signaux non initialisés\n",
                    m_id);
        return;
    }

    uint8_t typeH  = signal[1]->type(); // H
    uint8_t typeAH = signal[0]->type(); // AH

    CC_LOG_TRACE("[Canton %u][Signaux][CC] applyRoleDefaults rôle=%d | H=%d AH=%d\n",
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

    CC_LOG_INFO("[Canton %u][Signaux][CC] Types de signaux mis à jour selon le rôle\n",
                m_id);
}
