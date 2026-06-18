/*
 * Canton_Topologie.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Gestion de la topologie ferroviaire locale :
 *
 *   - indices SP1 / SM1 (voisins principaux)
 *   - états SP2 / SM2 (voies secondaires)
 *   - masques d’aiguilles bloquantes
 *   - accès aux CantonPeriph
 *   - helpers topologiques
 *   - déduction automatique du rôle SNCF (computeRole)
 *
 * IMPORTANT :
 *   - aucune logique métier ici
 *   - aucune logique d’aiguilles physiques
 *   - aucune logique de signaux
 *
 * Ce module décrit UNIQUEMENT la topologie locale du canton.
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
 *  Masques d’aiguilles bloquantes — SP1 / SM1 (principal)
 * ==========================================================================*/
void Canton::masqueAig(byte v)
{
    m_masqueAig = v;
    CC_LOG_TRACE("[Canton %u][Topo][CC] masqueAig = 0x%02X\n", m_id, m_masqueAig);
}

byte Canton::masqueAig()
{
    return m_masqueAig;
}

/* ============================================================================
 *  Masques d’aiguilles bloquantes — SP2 / SM2 (secondaire)
 * ==========================================================================*/
void Canton::masqueAigSP2(byte v)
{
    m_masqueAigSP2 = v;
    CC_LOG_TRACE("[Canton %u][Topo][CC] masqueAigSP2 = 0x%02X\n", m_id, m_masqueAigSP2);
}

byte Canton::masqueAigSP2()
{
    return m_masqueAigSP2;
}

void Canton::masqueAigSM2(byte v)
{
    m_masqueAigSM2 = v;
    CC_LOG_TRACE("[Canton %u][Topo][CC] masqueAigSM2 = 0x%02X\n", m_id, m_masqueAigSM2);
}

byte Canton::masqueAigSM2()
{
    return m_masqueAigSM2;
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
