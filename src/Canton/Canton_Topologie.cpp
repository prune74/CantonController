/*
 * Canton_Topologie.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Gestion de la topologie ferroviaire locale :
 *
 *   - indices SP1 / SM1 (voisins principaux)
 *   - indices SP2 / SM2 (voies secondaires)
 *   - accès aux CantonPeriph
 *   - helpers topologiques (accessibilité)
 *
 * IMPORTANT 2026 :
 *   - aucun masque d’aiguilles n’est maintenu dans Canton
 *   - les seuls masques valides sont dans CantonPeriph (voisins)
 *   - aucune logique métier ici
 *   - aucune logique de signaux
 */

#include "Canton.h"
#include "Config.h"
#include "debug_cc.h"

/* ============================================================================
 *  SP1 / SM1 — Voisins principaux
 * ==========================================================================*/

void Canton::SP1_idx(uint8_t idx)
{
    m_SP1_idx = idx;
    CC_LOG_TRACE("[Canton %u][Topo][CC] SP1_idx = %u\n", m_id, m_SP1_idx);
}

uint8_t Canton::SP1_idx()
{
    return m_SP1_idx;
}

void Canton::SM1_idx(uint8_t idx)
{
    m_SM1_idx = idx;
    CC_LOG_TRACE("[Canton %u][Topo][CC] SM1_idx = %u\n", m_id, m_SM1_idx);
}

uint8_t Canton::SM1_idx()
{
    return m_SM1_idx;
}

/* ============================================================================
 *  SP2 / SM2 — Voies secondaires (indices uniquement)
 * ==========================================================================*/

void Canton::SP2_idx(uint8_t idx)
{
    m_SP2_idx = idx;
    CC_LOG_TRACE("[Canton %u][Topo][CC] SP2_idx = %u\n", m_id, m_SP2_idx);
}

uint8_t Canton::SP2_idx()
{
    return m_SP2_idx;
}

void Canton::SM2_idx(uint8_t idx)
{
    m_SM2_idx = idx;
    CC_LOG_TRACE("[Canton %u][Topo][CC] SM2_idx = %u\n", m_id, m_SM2_idx);
}

uint8_t Canton::SM2_idx()
{
    return m_SM2_idx;
}

/* ============================================================================
 *  Accès aux voisins (CantonPeriph)
 * ==========================================================================*/

CantonPeriph *Canton::voisinSP1() const
{
    return getCantonP(m_SP1_idx);
}

CantonPeriph *Canton::voisinSM1() const
{
    return getCantonP(m_SM1_idx);
}

CantonPeriph *Canton::voisinSP2() const
{
    return getCantonP(m_SP2_idx);
}

CantonPeriph *Canton::voisinSM2() const
{
    return getCantonP(m_SM2_idx);
}

/* ============================================================================
 *  Helpers topologiques (accessibilité)
 * ==========================================================================*/

bool Canton::SP1_estAccessible()
{
    CantonPeriph *v = voisinSP1();
    return v && v->acces() && !v->busy();
}

bool Canton::SM1_estAccessible()
{
    CantonPeriph *v = voisinSM1();
    return v && v->acces() && !v->busy();
}

bool Canton::SP2_estAccessible()
{
    CantonPeriph *v = voisinSP2();
    return v && v->acces() && !v->busy();
}

bool Canton::SM2_estAccessible()
{
    CantonPeriph *v = voisinSM2();
    return v && v->acces() && !v->busy();
}
