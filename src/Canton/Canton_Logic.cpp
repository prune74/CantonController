/*
 * Canton_Logic.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Logique métier du canton :
 *   - règles d’accès ferroviaires internes
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
 *  Sélection du voisin selon la règle SNCF :
 *    - SP1 prioritaire
 *    - SP2 utilisé uniquement si SP1 impossible
 * ==========================================================================*/
CantonPeriph *Canton::choisirVoisin(SensDeMarche sens)
{
    CantonPeriph *v1 = (sens == SensHoraire) ? voisinSP1() : voisinSM1();
    CantonPeriph *v2 = (sens == SensHoraire) ? voisinSP2() : voisinSM2();

    const char *sensStr = (sens == SensHoraire) ? "H" : "AH";

    // 1) SP1 prioritaire
    if (v1 && v1->acces() && !v1->busy() && v1->reserved() == 0)
    {
        CC_LOG_TRACE("[Canton %u][Logic][CC] Voisin %s : SP1 choisi\n", m_id, sensStr);
        return v1;
    }

    // 2) SP2 en fallback
    if (v2 && v2->acces() && !v2->busy() && v2->reserved() == 0)
    {
        CC_LOG_TRACE("[Canton %u][Logic][CC] Voisin %s : SP2 choisi\n", m_id, sensStr);
        return v2;
    }

    CC_LOG_TRACE("[Canton %u][Logic][CC] Voisin %s : aucun voisin disponible\n", m_id, sensStr);
    return nullptr;
}

/* ============================================================================
 *  estAccesAutorise() — Règle générale d’accès
 * ---------------------------------------------------------------------------
 *  Conditions :
 *    1. STOP global inactif
 *    2. SP1 prioritaire, SP2 en fallback
 *    3. Voisin accessible
 *    4. Voisin libre (ni occupé ni réservé)
 * ==========================================================================*/
bool Canton::estAccesAutorise(SensDeMarche sens)
{
    const char *sensStr = (sens == SensHoraire) ? "H" : "AH";

    // 0) STOP global
    if (isStopActive())
    {
        CC_LOG_WARN("[Canton %u][Logic][CC] Accès %s BLOQUE (STOP actif)\n", m_id, sensStr);
        return false;
    }

    // 1) Sélection du voisin (SP1 → SP2)
    CantonPeriph *v = choisirVoisin(sens);
    if (!v)
    {
        CC_LOG_WARN("[Canton %u][Logic][CC] Accès %s refusé (aucun voisin valide)\n",
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
CantonPeriph *Canton::prochainVoisin(SensDeMarche sens)
{
    return choisirVoisin(sens);
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
bool Canton::peutEntrerDansVoisin(SensDeMarche sens)
{
    const char *sensStr = (sens == SensHoraire) ? "H" : "AH";

    // STOP global
    if (m_stopActive)
    {
        CC_LOG_WARN("[Canton %u][Logic][CC] Entrée %s BLOQUEE (STOP actif)\n",
                    m_id, sensStr);
        return false;
    }

    // Règle d'accès
    if (!estAccesAutorise(sens))
        return false;

    // Voisin choisi (SP1 → SP2)
    CantonPeriph *v = prochainVoisin(sens);
    if (!v)
        return false;

    // Voisin libre
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
bool Canton::estSortiePossible(SensDeMarche sens)
{
    const char *sensStr = (sens == SensHoraire) ? "H" : "AH";

    // STOP global
    if (m_stopActive)
    {
        CC_LOG_WARN("[Canton %u][Logic][CC] Sortie %s BLOQUEE (STOP actif)\n",
                    m_id, sensStr);
        return false;
    }

    // Le canton doit être occupé
    if (!m_busy)
        return false;

    // Vérifie l'entrée dans le voisin (SP1 → SP2)
    return peutEntrerDansVoisin(sens);
}
