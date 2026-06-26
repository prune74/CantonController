/*
 * Canton_SignauxTransitions.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Logique SNCF des transitions d’aspects :
 *
 *   - STOP global
 *   - sécurité absolue (occupation / réservation)
 *   - propagation des aspects restrictifs
 *   - prise en compte des voies secondaires (SP2 / SM2)
 *   - défaut → voie libre
 *
 * IMPORTANT 2026 :
 *   - La logique d’aspect dépend uniquement :
 *       → de la sécurité
 *       → de l’occupation des voisins (SP1/SP2 ou SM1/SM2)
 *       → de l’aspect reçu des voisins
 *   - Aucune logique métier externe ici
 *   - Aucune logique de mât ou de type de signal
 */

#include "Canton.h"
#include "Config.h"
#include "debug_cc.h"

/* ============================================================================
 *  Mini‑helpers internes
 * ==========================================================================*/

/**
 * Retourne l’aspect reçu d’un voisin dans le sens donné.
 * Si le voisin n’existe pas → CARRÉ (sécurité).
 */
static inline uint8_t aspectVoisin(CantonPeriph *v, SensDeMarche sens)
{
    if (!v)
        return ASPECT_CARRE;

    return (sens == SensHoraire)
               ? v->aspectRecu[0]  // aspect côté H
               : v->aspectRecu[1]; // aspect côté AH
}

/**
 * Retourne vrai si le voisin est occupé ou réservé.
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
 *    1. SP1 ou SP2 (ou SM1/SM2) occupé ou réservé → CARRÉ
 *    2. Propagation de l’aspect le plus restrictif des voisins
 *    3. Défaut → Voie libre
 *
 *  NOTE 2026 :
 *    - SP2/SM2 sont désormais intégrés dans la logique SNCF.
 *    - Le plus restrictif des deux voisins (principal + secondaire)
 *      est appliqué.
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
     * 1) Récupération des voisins (principal + secondaire)
     * ------------------------------------------------------------------------ */
    CantonPeriph *v1 = (sens == SensHoraire) ? voisinSP1() : voisinSM1();
    CantonPeriph *v2 = (sens == SensHoraire) ? voisinSP2() : voisinSM2();

    /* ------------------------------------------------------------------------
     * 2) Sécurité absolue : SP1 ou SP2 occupé → CARRÉ
     * ------------------------------------------------------------------------ */
    if (estOccupeOuReserve(v1) || estOccupeOuReserve(v2))
    {
        CC_LOG_TRACE("[Canton %u][Signaux][CC] transitionAspect(%s) → CARRÉ (voisin occupé)\n",
                     m_id, sensStr);
        return ASPECT_CARRE;
    }

    // ------------------------------------------------------------------------
    // 3) Propagation des aspects restrictifs
    // ------------------------------------------------------------------------
    uint8_t asp1 = aspectVoisin(v1, sens);
    uint8_t asp2 = aspectVoisin(v2, sens);

    // Ordre de sévérité décroissant
    static const uint8_t priorite[] = {
        ASPECT_CARRE,
        ASPECT_SEMAPHORE,
        ASPECT_AVERTISSEMENT};

    uint8_t asp = ASPECT_VOIE_LIBRE;

    for (uint8_t p : priorite)
    {
        if (asp1 == p || asp2 == p)
        {
            asp = p;
            break;
        }
    }

    if (asp != ASPECT_VOIE_LIBRE)
    {
        CC_LOG_TRACE("[Canton %u][Signaux][CC] transitionAspect(%s) → aspect propagé = %u\n",
                     m_id, sensStr, asp);
        return asp;
    }

    /* ------------------------------------------------------------------------
     * 4) Défaut → Voie libre
     * ------------------------------------------------------------------------ */
    CC_LOG_TRACE("[Canton %u][Signaux][CC] transitionAspect(%s) → Voie libre (défaut)\n",
                 m_id, sensStr);
    return ASPECT_VOIE_LIBRE;
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
