/*
 * Canton_SignauxTransitions.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Logique SNCF des transitions d’aspects :
 *
 *   - sécurité absolue (STOP, occupation)
 *   - propagation des aspects restrictifs
 *   - application des règles selon le rôle ferroviaire
 *
 * IMPORTANT :
 *   - aucune logique d’aiguilles ici
 *   - aucune logique directionnelle (feux blancs)
 *   - aucune logique d’accès
 *
 * Ce module définit UNIQUEMENT l’aspect SNCF à afficher.
 */

#include "Canton.h"
#include "Config.h"
#include "debug_cc.h"

/* ============================================================================
 *  Mini‑helpers internes
 * ==========================================================================*/

/*
 * aspectVoisin() — retourne l’aspect reçu depuis un voisin
 * sens = H  → aspectRecu[0]
 * sens = AH → aspectRecu[1]
 */
static inline uint8_t aspectVoisin(CantonPeriph *v, SensDeMarche sens)
{
    if (!v)
        return ASPECT_CARRE;

    return (sens == SensHoraire)
           ? v->aspectRecu[0]
           : v->aspectRecu[1];
}

/*
 * estOccupeOuReserve() — règle de sécurité
 */
static inline bool estOccupeOuReserve(CantonPeriph *v)
{
    return v && (v->busy() || v->reserved() != 0);
}

/* ============================================================================
 *  transitionAspect() — cœur de la logique SNCF
 * ---------------------------------------------------------------------------
 *  Ordre des règles :
 *
 *    0. STOP global → CARRÉ
 *    1. Voisin occupé ou réservé → CARRÉ
 *    2. Propagation des aspects restrictifs (CARRÉ / SÉMAPHORE / AVERTISSEMENT)
 *    3. Application du rôle ferroviaire
 *    4. Défaut → Voie libre
 * ==========================================================================*/
uint8_t Canton::transitionAspect(SensDeMarche sens)
{
    const char *sensStr = (sens == SensHoraire) ? "H" : "AH";

    /* ------------------------------------------------------------------------
     * 0) STOP global
     * ------------------------------------------------------------------------ */
    if (isStopActive())
    {
        CC_LOG_WARN("[Canton %u][Signaux][CC] transitionAspect(%s) → CARRÉ (STOP actif)\n",
                    m_id, sensStr);
        return ASPECT_CARRE;
    }

    /* ------------------------------------------------------------------------
     * Voisin principal (SP1 ou SM1)
     * ------------------------------------------------------------------------ */
    CantonPeriph *v = (sens == SensHoraire)
                      ? voisinSP1()
                      : voisinSM1();

    /* ------------------------------------------------------------------------
     * 1) Sécurité absolue : voisin occupé ou réservé → CARRÉ
     * ------------------------------------------------------------------------ */
    if (estOccupeOuReserve(v))
    {
        CC_LOG_TRACE("[Canton %u][Signaux][CC] transitionAspect(%s) → CARRÉ (voisin occupé)\n",
                     m_id, sensStr);
        return ASPECT_CARRE;
    }

    /* ------------------------------------------------------------------------
     * 2) Propagation des aspects restrictifs
     * ------------------------------------------------------------------------ */
    uint8_t aspV = aspectVoisin(v, sens);

    if (aspV == ASPECT_CARRE ||
        aspV == ASPECT_SEMAPHORE ||
        aspV == ASPECT_AVERTISSEMENT)
    {
        CC_LOG_TRACE("[Canton %u][Signaux][CC] transitionAspect(%s) → aspect voisin = %u\n",
                     m_id, sensStr, aspV);
        return aspV;
    }

    /* ------------------------------------------------------------------------
     * 3) Application des règles selon le rôle ferroviaire
     * ------------------------------------------------------------------------ */
    switch (m_role)
    {
        case ROLE_BAL:
            CC_LOG_TRACE("[Canton %u][Signaux][CC] transitionAspect(%s) → Voie libre (BAL)\n",
                         m_id, sensStr);
            return ASPECT_VOIE_LIBRE;

        case ROLE_ENTREE_GARE:
            CC_LOG_TRACE("[Canton %u][Signaux][CC] transitionAspect(%s) → Avertissement (entrée gare)\n",
                         m_id, sensStr);
            return ASPECT_AVERTISSEMENT;

        case ROLE_SORTIE_GARE:
            CC_LOG_TRACE("[Canton %u][Signaux][CC] transitionAspect(%s) → Voie libre (sortie gare)\n",
                         m_id, sensStr);
            return ASPECT_VOIE_LIBRE;

        case ROLE_MANOEUVRE:
            CC_LOG_TRACE("[Canton %u][Signaux][CC] transitionAspect(%s) → Manoeuvre\n",
                         m_id, sensStr);
            return ASPECT_MANOEUVRE;

        case ROLE_GARE:
        case ROLE_SERVICE:
        case ROLE_PLEINE_VOIE:
        default:
            CC_LOG_TRACE("[Canton %u][Signaux][CC] transitionAspect(%s) → Voie libre (défaut)\n",
                         m_id, sensStr);
            return ASPECT_VOIE_LIBRE;
    }
}

/* ============================================================================
 *  transitionH() / transitionAH()
 * ==========================================================================*/
uint8_t Canton::transitionH()
{
    return transitionAspect(SensHoraire);
}

uint8_t Canton::transitionAH()
{
    return transitionAspect(SensAntiHoraire);
}
