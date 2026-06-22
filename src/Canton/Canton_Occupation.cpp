/*
 * Canton_Occupation.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Gestion de l’occupation logique du canton :
 *   - busy()      : occupation physique (EXCC / courant)
 *   - reserved()  : réservation logique (adresse loco)
 *   - estOccupe() : synthèse occupation + réservation
 *   - maxSpeed()  : limitation locale de vitesse
 *   - sensMarche(): sens logique de circulation
 *
 * IMPORTANT :
 *   - aucune logique métier ici
 *   - aucune décision ferroviaire
 *   - ce module ne fait que stocker et exposer des états internes
 */

#include "Canton.h"
#include "Config.h"
#include "debug_cc.h"

/* ============================================================================
 *  busy() — Occupation physique
 * ---------------------------------------------------------------------------
 *  v = true  → canton occupé (courant détecté)
 *  v = false → canton libre
 * ==========================================================================*/
void Canton::busy(bool v) // 🟢
{
    m_busy = v;
    CC_LOG_TRACE("[Canton %u][Occ][CC] busy = %d\n", m_id, m_busy);
}

bool Canton::busy() // 🟢
{
    return m_busy;
}

/* ============================================================================
 *  reserved() — Réservation logique
 * ---------------------------------------------------------------------------
 *  addr = adresse DCC de la loco réservant le canton
 *  addr = 0 → aucune réservation
 * ==========================================================================*/
void Canton::reserved(uint16_t addr) // 🟢
{
    m_reserved = addr;
    CC_LOG_TRACE("[Canton %u][Occ][CC] reserved = %u\n", m_id, m_reserved);
}

uint16_t Canton::reserved() // 🟢
{
    return m_reserved;
}

/* ============================================================================
 *  estOccupe() — Synthèse occupation + réservation
 * ---------------------------------------------------------------------------
 *  Retourne true si :
 *    - courant détecté (busy)
 *    - OU réservation active (reserved != 0)
 * ==========================================================================*/
bool Canton::estOccupe() // 🟢
{
    return (m_busy || m_reserved != 0);
}

/* ============================================================================
 *  maxSpeed() — Limitation locale de vitesse
 * ---------------------------------------------------------------------------
 *  Valeur utilisée par la supervision pour brider la loco.
 * ==========================================================================*/
void Canton::maxSpeed(uint8_t v) // 🟢
{
    m_maxSpeed = v;
    CC_LOG_TRACE("[Canton %u][Occ][CC] maxSpeed = %u\n", m_id, m_maxSpeed);
}

uint8_t Canton::maxSpeed() // 🟢
{
    return m_maxSpeed;
}

/* ============================================================================
 *  sensMarche() — Sens logique de circulation (Exploration 2026)
 * ---------------------------------------------------------------------------
 *  SensHoraire / SensAntiHoraire
 * ==========================================================================*/
void Canton::sensMarche(SensDeMarche v) // 🟢
{
    m_sensMarche = v;
    CC_LOG_TRACE("[Canton %u][Occ][CC] sensMarche = %u\n",
                 m_id, (uint8_t)m_sensMarche);
}

SensDeMarche Canton::sensMarche() // 🟢
{
    return m_sensMarche;
}
