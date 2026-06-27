/*
 * Canton_SignauxTransitions.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Logique SNCF des transitions d’aspects :
 */

#include "Canton.h"
#include "Config.h"
#include "debug_cc.h"

/* ============================================================================
 *  Mini‑helpers internes
 * ==========================================================================*/

static inline ExccAspect aspectVoisin(CantonPeriph *v, SensDeMarche sens)
{
    if (!v)
        return ExccAspect::ASPECT_CARRE;

    return (sens == SensHoraire)
           ? static_cast<ExccAspect>(v->aspectRecu[0])
           : static_cast<ExccAspect>(v->aspectRecu[1]);
}

static inline bool estOccupeOuReserve(CantonPeriph *v)
{
    return v && (v->busy() || v->reserved() != 0);
}

/* ============================================================================
 *  transitionAspect() — cœur de la logique SNCF
 * ==========================================================================*/
uint8_t Canton::transitionAspect(SensDeMarche sens)
{
    const char *sensStr = (sens == SensHoraire) ? "H" : "AH";

    /* 0) STOP global */
    if (isStopActive())
    {
        CC_LOG_WARN("[Canton %u][Signaux][CC] transitionAspect(%s) → CARRÉ (STOP actif)\n",
                    m_id, sensStr);
        return static_cast<uint8_t>(ExccAspect::ASPECT_CARRE);
    }

    /* Voisin principal */
    CantonPeriph *v = (sens == SensHoraire)
                      ? voisinSP1()
                      : voisinSM1();

    /* 1) Sécurité absolue : voisin occupé ou réservé → CARRÉ */
    if (estOccupeOuReserve(v))
    {
        CC_LOG_TRACE("[Canton %u][Signaux][CC] transitionAspect(%s) → CARRÉ (voisin occupé)\n",
                     m_id, sensStr);
        return static_cast<uint8_t>(ExccAspect::ASPECT_CARRE);
    }

    /* 2) Propagation des aspects restrictifs */
    ExccAspect aspV = aspectVoisin(v, sens);

    if (aspV == ExccAspect::ASPECT_CARRE ||
        aspV == ExccAspect::ASPECT_SEMAPHORE ||
        aspV == ExccAspect::ASPECT_AVERTISSEMENT)
    {
        CC_LOG_TRACE("[Canton %u][Signaux][CC] transitionAspect(%s) → aspect voisin = %u\n",
                     m_id, sensStr, static_cast<uint8_t>(aspV));
        return static_cast<uint8_t>(aspV);
    }

    /* 3) Défaut → Voie libre */
    CC_LOG_TRACE("[Canton %u][Signaux][CC] transitionAspect(%s) → Voie libre (défaut)\n",
                 m_id, sensStr);
    return static_cast<uint8_t>(ExccAspect::ASPECT_VOIE_LIBRE);
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
