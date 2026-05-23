/*
 * Node_Occupation.cpp — Gestion de l’occupation du canton
 */

#include "Node_Internal.h"
#include "debug_sa.h"

/* ============================================================================
 * busy()
 * ============================================================================
 */

void Node::busy(bool v)
{
    m_busy = v;
    SA_LOG_TRACE("[Node %u] busy() = %d\n", m_id, m_busy);
}

bool Node::busy()
{
    return m_busy;
}

/* ============================================================================
 * reserved()
 * ============================================================================
 */

void Node::reserved(uint16_t addr)
{
    m_reserved = addr;
    SA_LOG_TRACE("[Node %u] reserved() = %u\n", m_id, m_reserved);
}

uint16_t Node::reserved()
{
    return m_reserved;
}

/* ============================================================================
 * estOccupe()
 * ============================================================================
 */

bool Node::estOccupe()
{
    return (m_busy || m_reserved != 0);
}

/* ============================================================================
 * maxSpeed()
 * ============================================================================
 */

void Node::maxSpeed(uint8_t v)
{
    m_maxSpeed = v;
    SA_LOG_TRACE("[Node %u] maxSpeed() = %u\n", m_id, m_maxSpeed);
}

uint8_t Node::maxSpeed()
{
    return m_maxSpeed;
}

/* ============================================================================
 * sensMarche() — Version Discovery 2026
 * ============================================================================
 */

void Node::sensMarche(SensDeMarche v)
{
    m_sensMarche = v;
    SA_LOG_TRACE("[Node %u] sensMarche() = %u\n", m_id, (uint8_t)m_sensMarche);
}

SensDeMarche Node::sensMarche()
{
    return m_sensMarche;
}
