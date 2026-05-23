/*
 * Node_Topologie.cpp — Gestion de la topologie ferroviaire du canton
 */

#include "Node_Internal.h"
#include "debug_sa.h"

/* ============================================================================
 * SP1 / SM1 — Voisins principaux
 * ============================================================================
 */

void Node::SP1_idx(uint8_t idx)
{
    m_SP1_idx = idx;
    SA_LOG_TRACE("[Node %u] SP1_idx défini = %u\n", m_id, m_SP1_idx);
}

uint8_t Node::SP1_idx()
{
    return m_SP1_idx;
}

void Node::SM1_idx(uint8_t idx)
{
    m_SM1_idx = idx;
    SA_LOG_TRACE("[Node %u] SM1_idx défini = %u\n", m_id, m_SM1_idx);
}

uint8_t Node::SM1_idx()
{
    return m_SM1_idx;
}

/* ============================================================================
 * SP2 / SM2 — Voies secondaires
 * ============================================================================
 */

void Node::SP2_acces(bool v)
{
    m_SP2_acces = v;
    SA_LOG_TRACE("[Node %u] SP2_acces = %d\n", m_id, m_SP2_acces);
}

bool Node::SP2_acces()
{
    return m_SP2_acces;
}

void Node::SP2_busy(bool v)
{
    m_SP2_busy = v;
}

bool Node::SP2_busy()
{
    return m_SP2_busy;
}

void Node::SM2_acces(bool v)
{
    m_SM2_acces = v;
    SA_LOG_TRACE("[Node %u] SM2_acces = %d\n", m_id, m_SM2_acces);
}

bool Node::SM2_acces()
{
    return m_SM2_acces;
}

void Node::SM2_busy(bool v)
{
    m_SM2_busy = v;
}

bool Node::SM2_busy()
{
    return m_SM2_busy;
}

/* ============================================================================
 * Masques d’aiguilles bloquantes — SP1 / SM1 (principal)
 * ============================================================================
 */

void Node::masqueAig(byte v)
{
    m_masqueAig = v;
    SA_LOG_TRACE("[Node %u] masqueAig = 0x%02X\n", m_id, m_masqueAig);
}

byte Node::masqueAig()
{
    return m_masqueAig;
}

/* ============================================================================
 * Masques d’aiguilles bloquantes — SP2 / SM2 (secondaire)
 * ============================================================================
 */

void Node::masqueAigSP2(byte v)
{
    m_masqueAigSP2 = v;
    SA_LOG_TRACE("[Node %u] masqueAigSP2 = 0x%02X\n", m_id, m_masqueAigSP2);
}

byte Node::masqueAigSP2()
{
    return m_masqueAigSP2;
}

void Node::masqueAigSM2(byte v)
{
    m_masqueAigSM2 = v;
    SA_LOG_TRACE("[Node %u] masqueAigSM2 = 0x%02X\n", m_id, m_masqueAigSM2);
}

byte Node::masqueAigSM2()
{
    return m_masqueAigSM2;
}

/* ============================================================================
 * Accès aux voisins (nodeP[])
 * ============================================================================
 */

NodePeriph* Node::voisinSP1()
{
    return getNodeP(m_SP1_idx);
}

NodePeriph* Node::voisinSM1()
{
    return getNodeP(m_SM1_idx);
}

NodePeriph* Node::voisinSP2()
{
    return getNodeP(2);
}

NodePeriph* Node::voisinSM2()
{
    return getNodeP(3);
}

/* ============================================================================
 * Mini‑helpers topologiques
 * ============================================================================
 */

bool Node::SP1_estAccessible()
{
    NodePeriph* v = voisinSP1();
    return v && v->acces() && !v->busy();
}

bool Node::SM1_estAccessible()
{
    NodePeriph* v = voisinSM1();
    return v && v->acces() && !v->busy();
}

bool Node::SP2_estAccessible()
{
    return m_SP2_acces && !m_SP2_busy;
}

bool Node::SM2_estAccessible()
{
    return m_SM2_acces && !m_SM2_busy;
}
