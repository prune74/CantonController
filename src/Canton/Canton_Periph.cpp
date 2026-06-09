/*
 * Canton_Periph.cpp — Gestion des voisins d’un canton (CantonPeriph)
 * ---------------------------------------------------------------------------
 * Rôle :
 *   CantonPeriph représente un "voisin" dans la topologie ferroviaire :
 *
 *       SP1 / SP2 / SM1 / SM2
 *
 *   Chaque CantonPeriph correspond à un canton adjacent et stocke :
 *     - son ID
 *     - son occupation logique (busy)
 *     - son autorisation d’accès (acces)
 *     - la loco réservée dans ce voisin (reserved)
 *     - l’adresse Railcom de la loco présente (locoAddr)
 *     - les aiguilles bloquantes pour accéder à ce voisin (masqueAig)
 *     - les aspects reçus depuis EXSA (aspectRecu[2])
 */

#include "Canton.h"
#include "Config.h"
#include "debug_sa.h"

// Compteur d’instances (debug)
uint8_t CantonPeriph::comptInst = 0;

/*
 * Constructeur
 * ---------------------------------------------------------------------------
 * Initialise toutes les valeurs internes à un état neutre.
 */
CantonPeriph::CantonPeriph()
    : m_id(NODE_UNUSED_ID),
      m_busy(false),
      m_reserved(0),
      m_acces(true),
      m_locoAddr(0),
      m_masqueAig(0x00),
      m_signal(0)
{
    // Aspects reçus depuis EXSA (H et AH)
    aspectRecu[0] = 0;
    aspectRecu[1] = 0;

    ++comptInst;
    SA_LOG_TRACE("[CantonPeriph] Nouvelle instance (total=%u)\n", comptInst);
}

/*
 * Destructeur
 * ---------------------------------------------------------------------------
 */
CantonPeriph::~CantonPeriph()
{
    --comptInst;
    SA_LOG_TRACE("[CantonPeriph] Destruction instance (restant=%u)\n", comptInst);
}

/* ============================================================================
 * Getters / Setters
 * ============================================================================
 */

void CantonPeriph::ID(uint8_t id)
{
    m_id = id;
    SA_LOG_TRACE("[CantonPeriph] ID défini = %u\n", m_id);
}

uint8_t CantonPeriph::ID()
{
    return m_id;
}

void CantonPeriph::busy(bool busy)
{
    m_busy = busy;
}

bool CantonPeriph::busy()
{
    return m_busy;
}

void CantonPeriph::reserved(uint16_t locoAddr)
{
    m_reserved = locoAddr;
}

uint16_t CantonPeriph::reserved()
{
    return m_reserved;
}

void CantonPeriph::acces(bool acces)
{
    m_acces = acces;
}

bool CantonPeriph::acces()
{
    return m_acces;
}

void CantonPeriph::locoAddr(uint16_t addr)
{
    m_locoAddr = addr;
}

uint16_t CantonPeriph::locoAddr()
{
    return m_locoAddr;
}

void CantonPeriph::masqueAig(byte masqueAig)
{
    m_masqueAig = masqueAig;
}

byte CantonPeriph::masqueAig()
{
    return m_masqueAig;
}
