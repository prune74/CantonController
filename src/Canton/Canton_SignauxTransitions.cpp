/*
 * Canton_SignauxTransitions.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Logique SNCF des transitions d’aspects :
 *
 *   - sécurité absolue (STOP, occupation)
 *   - propagation des aspects restrictifs
 *   - défaut → voie libre
 *
 * IMPORTANT:
 *   - La logique d’aspect dépend uniquement :
 *       → de la sécurité
 *       → de l’occupation
 *       → de l’aspect du voisin
 */

#include "Canton.h"
#include "Config.h"
#include "debug_cc.h"

/* ============================================================================
 *  Mini‑helpers internes
 * ==========================================================================*/

static inline uint8_t aspectVoisin(CantonPeriph *v, SensDeMarche sens) // 🟢
{
    if (!v)
        return ASPECT_CARRE;

    return (sens == SensHoraire)
           ? v->aspectRecu[0]
           : v->aspectRecu[1];
}

static inline bool estOccupeOuReserve(CantonPeriph *v) // 🟢
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
 *    2. Propagation des aspects restrictifs
 *    3. Défaut → Voie libre
 * ==========================================================================*/
uint8_t Canton::transitionAspect(SensDeMarche sens) // 🟢
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
     * Voisin principal
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
     * 3) Défaut → Voie libre
     * ------------------------------------------------------------------------ */
    CC_LOG_TRACE("[Canton %u][Signaux][CC] transitionAspect(%s) → Voie libre (défaut)\n",
                 m_id, sensStr);
    return ASPECT_VOIE_LIBRE;
}

/* ============================================================================
 *  transitionH() / transitionAH()
 * ==========================================================================*/
uint8_t Canton::transitionH() // 🟢
{
    return transitionAspect(SensHoraire);
}

uint8_t Canton::transitionAH() // 🟢
{
    return transitionAspect(SensAntiHoraire);
}
