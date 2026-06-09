/*
 * Canton_SignauxTransitions.cpp — Logique SNCF des transitions d’aspects
 */

#include "Canton.h"
#include "Config.h"
#include "debug_sa.h"

/* ============================================================================
 * Mini‑helpers internes
 * ============================================================================
 */

/*
 * aspectVoisin() — retourne l’aspect reçu depuis un voisin
 * sens = SensHoraire     → aspectRecu[0]
 * sens = SensAntiHoraire → aspectRecu[1]
 */
static inline uint8_t aspectVoisin(CantonPeriph *v, SensDeMarche sens)
{
    if (!v)
        return ASPECT_CARRE;

    return (sens == SensHoraire) ? v->aspectRecu[0]
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
 * transitionAspect() — cœur de la logique SNCF
 * ============================================================================
 */

uint8_t Canton::transitionAspect(SensDeMarche sens)
{
    // 🔥 STOP global Exploration 2026 : tous les signaux = CARRÉ
    if (isStopActive())
    {
        SA_LOG_WARN("[Canton %u] transitionAspect(%s) → CARRÉ (STOP actif)\n",
                    m_id,
                    (sens == SensHoraire ? "H" : "AH"));
        return ASPECT_CARRE;
    }

    CantonPeriph *v = (sens == SensHoraire)
                          ? voisinSP1()
                          : voisinSM1();

    const char *sensStr = (sens == SensHoraire) ? "H" : "AH";

    /*
     * 1) Sécurité absolue : canton suivant occupé → CARRÉ
     */
    if (estOccupeOuReserve(v))
    {
        SA_LOG_TRACE("[Canton %u] transitionAspect(%s) → CARRÉ (voisin occupé)\n",
                     m_id, sensStr);
        return ASPECT_CARRE;
    }

    /*
     * 2) Si le voisin impose un aspect restrictif → on le relaie
     */
    uint8_t aspV = aspectVoisin(v, sens);

    if (aspV == ASPECT_CARRE ||
        aspV == ASPECT_SEMAPHORE ||
        aspV == ASPECT_AVERTISSEMENT)
    {
        SA_LOG_TRACE("[Canton %u] transitionAspect(%s) → aspect voisin = %u\n",
                     m_id, sensStr, aspV);
        return aspV;
    }

    /*
     * 3) Application des règles selon le rôle ferroviaire
     */
    switch (m_role)
    {
    case ROLE_BAL:
        SA_LOG_TRACE("[Canton %u] transitionAspect(%s) → Voie libre (BAL)\n",
                     m_id, sensStr);
        return ASPECT_VOIE_LIBRE;

    case ROLE_ENTREE_GARE:
        SA_LOG_TRACE("[Canton %u] transitionAspect(%s) → Avertissement (entrée gare)\n",
                     m_id, sensStr);
        return ASPECT_AVERTISSEMENT;

    case ROLE_SORTIE_GARE:
        SA_LOG_TRACE("[Canton %u] transitionAspect(%s) → Voie libre (sortie gare)\n",
                     m_id, sensStr);
        return ASPECT_VOIE_LIBRE;

    case ROLE_MANOEUVRE:
        SA_LOG_TRACE("[Canton %u] transitionAspect(%s) → Manoeuvre\n",
                     m_id, sensStr);
        return ASPECT_MANOEUVRE;

    case ROLE_GARE:
    case ROLE_SERVICE:
    case ROLE_PLEINE_VOIE:
    default:
        SA_LOG_TRACE("[Canton %u] transitionAspect(%s) → Voie libre (défaut)\n",
                     m_id, sensStr);
        return ASPECT_VOIE_LIBRE;
    }
}

/* ============================================================================
 * transitionH() / transitionAH()
 * ============================================================================
 */

uint8_t Canton::transitionH()
{
    return transitionAspect(SensHoraire);
}

uint8_t Canton::transitionAH()
{
    return transitionAspect(SensAntiHoraire);
}
