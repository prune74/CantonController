/*
 * Canton_Logic.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Logique métier du canton :
 *   - règles d’accès
 *   - cohérence topologique
 *   - sécurité (STOP, occupation, réservations)
 *
 * IMPORTANT :
 *   - aucune logique d’aiguilles physiques ici
 *   - aucune logique de signaux
 *   - aucune logique directionnelle
 *
 * Ce module définit UNIQUEMENT les règles d’accès ferroviaires internes.
 */

#include "Canton.h"
#include "Config.h"
#include "debug_cc.h"

/* ============================================================================
 *  estAccesAutorise() — Règle générale d’accès
 * ---------------------------------------------------------------------------
 *  Conditions :
 *    1. STOP global inactif
 *    2. Le voisin existe
 *    3. Le voisin est accessible
 *    4. Le voisin n’est ni occupé ni réservé
 * ==========================================================================*/
bool Canton::estAccesAutorise(SensDeMarche sens) // 🟢

{
    // 0) STOP global
    if (isStopActive())
    {
        CC_LOG_WARN("[Canton %u][Logic][CC] Accès %s BLOQUE (STOP actif)\n",
                    m_id,
                    (sens == SensHoraire ? "H" : "AH"));
        return false;
    }

    CantonPeriph *v = (sens == SensHoraire) ? voisinSP1() : voisinSM1();
    const char *sensStr = (sens == SensHoraire) ? "H" : "AH";

    // 1) Voisin existant
    if (!v)
    {
        CC_LOG_WARN("[Canton %u][Logic][CC] Accès %s refusé (voisin inexistant)\n",
                    m_id, sensStr);
        return false;
    }

    // 2) Voisin accessible
    if (!v->acces())
    {
        CC_LOG_TRACE("[Canton %u][Logic][CC] Accès %s refusé (acces=0)\n", m_id, sensStr);
        return false;
    }

    // 3) Voisin libre
    if (v->busy() || v->reserved() != 0)
    {
        CC_LOG_TRACE("[Canton %u][Logic][CC] Accès %s refusé (voisin occupé/réservé)\n",
                     m_id, sensStr);
        return false;
    }

    return true;
}

/* ============================================================================
 *  prochainVoisin() — Détermine le prochain canton selon le sens
 * ==========================================================================*/
CantonPeriph *Canton::prochainVoisin(SensDeMarche sens) // 🟢
{
    return (sens == SensHoraire) ? voisinSP1() : voisinSM1();
}

/* ============================================================================
 *  peutEntrerDansVoisin() — Vérifie si une loco peut entrer dans le voisin
 * ---------------------------------------------------------------------------
 *  Conditions :
 *    - STOP inactif
 *    - accès autorisé
 *    - voisin existant
 *    - voisin libre
 * ==========================================================================*/
bool Canton::peutEntrerDansVoisin(SensDeMarche sens) // 🟢
{
    if (m_stopActive)
    {
        CC_LOG_WARN("[Canton %u][Logic][CC] Entrée %s BLOQUEE (STOP actif)\n",
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
 *  estSortiePossible() — Vérifie si la loco peut quitter ce canton
 * ---------------------------------------------------------------------------
 *  Conditions :
 *    - STOP inactif
 *    - canton occupé
 *    - entrée dans le voisin possible
 * ==========================================================================*/
bool Canton::estSortiePossible(SensDeMarche sens) // 🟢

{
    // 🔥 STOP global Exploration 2026 : aucune sortie autorisée
    if (m_stopActive)
    {
        CC_LOG_WARN("[Canton %u][Logic][CC] Sortie %s BLOQUEE (STOP actif)\n",
                    m_id,
                    (sens == SensHoraire ? "H" : "AH"));
        return false;
    }

    if (!m_busy)
        return false;

    return peutEntrerDansVoisin(sens);
}
