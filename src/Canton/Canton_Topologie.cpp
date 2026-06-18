/*
 * Canton_Topologie.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Gestion de la topologie ferroviaire locale :
 *
 *   - indices SP1 / SM1 (voisins principaux)
 *   - états SP2 / SM2 (voies secondaires)
 *   - accès aux CantonPeriph
 *   - helpers topologiques
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
 *  SP2 / SM2 — Voies secondaires
 * ==========================================================================*/
void Canton::SP2_acces(bool v)
{
    m_SP2_acces = v;
    CC_LOG_TRACE("[Canton %u][Topo][CC] SP2_acces = %d\n", m_id, m_SP2_acces);
}

bool Canton::SP2_acces()
{
    return m_SP2_acces;
}

void Canton::SP2_busy(bool v)
{
    m_SP2_busy = v;
}

bool Canton::SP2_busy()
{
    return m_SP2_busy;
}

void Canton::SM2_acces(bool v)
{
    m_SM2_acces = v;
    CC_LOG_TRACE("[Canton %u][Topo][CC] SM2_acces = %d\n", m_id, m_SM2_acces);
}

bool Canton::SM2_acces()
{
    return m_SM2_acces;
}

void Canton::SM2_busy(bool v)
{
    m_SM2_busy = v;
}

bool Canton::SM2_busy()
{
    return m_SM2_busy;
}

/* ============================================================================
 *  Accès aux voisins (cantonP[])
 * ==========================================================================*/
CantonPeriph *Canton::voisinSP1()
{
    return getCantonP(m_SP1_idx);
}

CantonPeriph *Canton::voisinSM1()
{
    return getCantonP(m_SM1_idx);
}

CantonPeriph *Canton::voisinSP2()
{
    return getCantonP(2);
}

CantonPeriph *Canton::voisinSM2()
{
    return getCantonP(3);
}

/* ============================================================================
 *  Mini‑helpers topologiques
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
    return m_SP2_acces && !m_SP2_busy;
}

bool Canton::SM2_estAccessible()
{
    return m_SM2_acces && !m_SM2_busy;
}
