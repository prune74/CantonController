/*
 * Canton_Logic.cpp — Logique métier du canton (accès, sécurité, cohérence)
 */

#include "Canton.h"
#include "Config.h"
#include "debug_sa.h"

/* ============================================================================
 * estAccesAutorise() — Règle générale d’accès
 * ============================================================================
 */

bool Canton::estAccesAutorise(SensDeMarche sens)
{
    // 🔥 STOP global Exploration 2026 : aucun accès autorisé
    if (isStopActive())
    {
        SA_LOG_WARN("[Canton %u] Accès %s BLOQUE (STOP actif)\n",
                    m_id,
                    (sens == SensHoraire ? "H" : "AH"));
        return false;
    }

    CantonPeriph *v = nullptr;
    byte masque = m_masqueAig;

    if (sens == SensHoraire)
        v = voisinSP1();
    else
        v = voisinSM1();

    const char *sensStr = (sens == SensHoraire) ? "H" : "AH";

    // 1) Le rôle doit autoriser l’accès
    if (!roleAutoriseAcces(sens))
    {
        SA_LOG_TRACE("[Canton %u] Accès %s refusé (rôle)\n", m_id, sensStr);
        return false;
    }

    // 2) Le voisin doit exister
    if (!v)
    {
        SA_LOG_WARN("[Canton %u] Accès %s refusé (voisin inexistant)\n", m_id, sensStr);
        return false;
    }

    // 3) Le voisin doit être accessible
    if (!v->acces())
    {
        SA_LOG_TRACE("[Canton %u] Accès %s refusé (acces=0)\n", m_id, sensStr);
        return false;
    }

    // 4) Le voisin ne doit pas être occupé ou réservé
    if (v->busy() || v->reserved() != 0)
    {
        SA_LOG_TRACE("[Canton %u] Accès %s refusé (voisin occupé/réservé)\n", m_id, sensStr);
        return false;
    }

    // 5) Vérification des masques d’aiguilles
    if (!aiguillesConformes(masque))
    {
        SA_LOG_TRACE("[Canton %u] Accès %s refusé (masque aiguilles)\n", m_id, sensStr);
        return false;
    }

    return true;
}

/* ============================================================================
 * aiguillesConformes() — Vérifie les masques d’aiguilles
 * ============================================================================
 */

bool Canton::aiguillesConformes(byte masque)
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
            SA_LOG_TRACE("[Canton %u] Aiguille %u non conforme au masque\n", m_id, i);
            return false;
        }
    }

    return true;
}

/* ============================================================================
 * prochainVoisin() — Détermine le prochain canton selon le sens
 * ============================================================================
 */

CantonPeriph *Canton::prochainVoisin(SensDeMarche sens)
{
    return (sens == SensHoraire) ? voisinSP1() : voisinSM1();
}

/* ============================================================================
 * peutEntrerDansVoisin() — Vérifie si une loco peut entrer dans le voisin
 * ============================================================================
 */

bool Canton::peutEntrerDansVoisin(SensDeMarche sens)
{
    // 🔥 STOP global Exploration 2026 : aucune entrée autorisée
    if (m_stopActive)
    {
        SA_LOG_WARN("[Canton %u] Entrée %s BLOQUEE (STOP actif)\n",
                    m_id,
                    (sens == SensHoraire ? "H" : "AH"));
        return false;
    }

    if (!estAccesAutorise(sens))
        return false;

    CantonPeriph *v = prochainVoisin(sens);
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

bool Canton::estSortiePossible(SensDeMarche sens)
{
    // 🔥 STOP global Exploration 2026 : aucune sortie autorisée
    if (m_stopActive)
    {
        SA_LOG_WARN("[Canton %u] Sortie %s BLOQUEE (STOP actif)\n",
                    m_id,
                    (sens == SensHoraire ? "H" : "AH"));
        return false;
    }

    if (!m_busy)
        return false;

    return peutEntrerDansVoisin(sens);
}
