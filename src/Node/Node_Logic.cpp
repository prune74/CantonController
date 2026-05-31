/*
 * Node_Logic.cpp — Logique métier du canton (accès, sécurité, cohérence)
 */

#include "Node_Internal.h"
#include "debug_sa.h"

/* ============================================================================
 * estAccesAutorise() — Règle générale d’accès
 * ============================================================================
 */

bool Node::estAccesAutorise(SensDeMarche sens)
{
    // 🔥 STOP global Discovery 2026 : aucun accès autorisé
    if (isStopActive())
    {
        SA_LOG_WARN("[Node %u] Accès %s BLOQUE (STOP actif)\n",
                    m_id,
                    (sens == SensHoraire ? "H" : "AH"));
        return false;
    }

    NodePeriph *v = nullptr;
    byte masque = 0;

    if (sens == SensHoraire)
    {
        v = voisinSP1();
        masque = m_masqueAig;
    }
    else // SensAntiHoraire
    {
        v = voisinSM1();
        masque = m_masqueAig;
    }

    const char *sensStr = (sens == SensHoraire) ? "H" : "AH";

    // 1) Le rôle doit autoriser l’accès
    if (!roleAutoriseAcces(sens))
    {
        SA_LOG_TRACE("[Node %u] Accès %s refusé (rôle)\n", m_id, sensStr);
        return false;
    }

    // 2) Le voisin doit exister
    if (!v)
    {
        SA_LOG_WARN("[Node %u] Accès %s refusé (voisin inexistant)\n", m_id, sensStr);
        return false;
    }

    // 3) Le voisin doit être accessible
    if (!v->acces())
    {
        SA_LOG_TRACE("[Node %u] Accès %s refusé (acces=0)\n", m_id, sensStr);
        return false;
    }

    // 4) Le voisin ne doit pas être occupé ou réservé
    if (v->busy() || v->reserved() != 0)
    {
        SA_LOG_TRACE("[Node %u] Accès %s refusé (voisin occupé/réservé)\n", m_id, sensStr);
        return false;
    }

    // 5) Vérification des masques d’aiguilles
    if (!aiguillesConformes(masque))
    {
        SA_LOG_TRACE("[Node %u] Accès %s refusé (masque aiguilles)\n", m_id, sensStr);
        return false;
    }

    return true;
}

/* ============================================================================
 * aiguillesConformes() — Vérifie les masques d’aiguilles
 * ============================================================================
 */

bool Node::aiguillesConformes(byte masque)
{
    for (uint8_t i = 0; i < 4; i++)
    {
        if (!(masque & (1 << i)))
            continue; // aiguille non concernée

        Aig *a = getAig(i);
        if (!a)
            continue; // aiguille non configurée

        if (!a->estDroit())
        {
            SA_LOG_TRACE("[Node %u] Aiguille %u non conforme au masque\n", m_id, i);
            return false;
        }
    }

    return true;
}

/* ============================================================================
 * prochainVoisin() — Détermine le prochain canton selon le sens
 * ============================================================================
 */

NodePeriph *Node::prochainVoisin(SensDeMarche sens)
{
    if (sens == SensHoraire)
        return voisinSP1();

    return voisinSM1();
}

/* ============================================================================
 * peutEntrerDansVoisin() — Vérifie si une loco peut entrer dans le voisin
 * ============================================================================
 */

bool Node::peutEntrerDansVoisin(SensDeMarche sens)
{
    // 🔥 STOP global Discovery 2026 : aucune entrée autorisée
    if (m_stopActive)
    {
        SA_LOG_WARN("[Node %u] Entrée %s BLOQUEE (STOP actif)\n",
                    m_id,
                    (sens == SensHoraire ? "H" : "AH"));
        return false;
    }

    if (!estAccesAutorise(sens))
        return false;

    NodePeriph *v = prochainVoisin(sens);
    if (!v)
        return false;

    if (v->busy() || v->reserved() != 0)
        return false;

    return true;
}

/* ============================================================================
 * estSortiePossible() — Vérifie si la loco peut quitter ce canton
 * ============================================================================
 */

bool Node::estSortiePossible(SensDeMarche sens)
{
    // 🔥 STOP global Discovery 2026 : aucune sortie autorisée
    if (m_stopActive)
    {
        SA_LOG_WARN("[Node %u] Sortie %s BLOQUEE (STOP actif)\n",
                    m_id,
                    (sens == SensHoraire ? "H" : "AH"));
        return false;
    }

    if (!m_busy)
        return false;

    return peutEntrerDansVoisin(sens);
}
