/*
 * Canton_Occupation.cpp — Gestion de l’occupation du canton
 */

#include "Canton.h"
#include "Config.h"
#include "debug_sa.h"

/* ============================================================================
 * busy()
 * ============================================================================
 */

void Canton::busy(bool v)
{
    m_busy = v;
    SA_LOG_TRACE("[Canton %u] busy() = %d\n", m_id, m_busy);
}

bool Canton::busy()
{
    return m_busy;
}

/* ============================================================================
 * reserved()
 * ============================================================================
 */

void Canton::reserved(uint16_t addr)
{
    m_reserved = addr;
    SA_LOG_TRACE("[Canton %u] reserved() = %u\n", m_id, m_reserved);
}

uint16_t Canton::reserved()
{
    return m_reserved;
}

/* ============================================================================
 * estOccupe()
 * ============================================================================
 */

bool Canton::estOccupe()
{
    return (m_busy || m_reserved != 0);
}

/* ============================================================================
 * maxSpeed()
 * ============================================================================
 */

void Canton::maxSpeed(uint8_t v)
{
    m_maxSpeed = v;
    SA_LOG_TRACE("[Canton %u] maxSpeed() = %u\n", m_id, m_maxSpeed);
}

uint8_t Canton::maxSpeed()
{
    return m_maxSpeed;
}

/* ============================================================================
 * sensMarche() — Version Exploration 2026
 * ============================================================================
 */

void Canton::sensMarche(SensDeMarche v)
{
    m_sensMarche = v;
    SA_LOG_TRACE("[Canton %u] sensMarche() = %u\n", m_id, (uint8_t)m_sensMarche);
}

SensDeMarche Canton::sensMarche()
{
    return m_sensMarche;
}
