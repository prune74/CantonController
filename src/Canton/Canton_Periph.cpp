/*
 * Canton_Periph.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Représentation d’un voisin ferroviaire dans la topologie locale :
 *
 *     SP1 / SP2 / SM1 / SM2
 *
 * Un CantonPeriph correspond à un canton adjacent et stocke :
 *   - ID du voisin
 *   - occupation logique (busy)
 *   - autorisation d’accès (acces)
 *   - réservation (reserved)
 *   - adresse RailCom de la loco présente (locoAddr)
 *   - masque d’aiguilles dangereuses (masqueAigTopo)
 *   - aspects reçus depuis EXCC (aspectRecu[H/AH])
 *
 * IMPORTANT 2026 :
 *   - ce masque est le SEUL masque d’aiguilles utilisé dans le système
 *   - il provient du message CAN 0xC1 pendant l’Exploration
 *   - il est utilisé par la logique métier (danger, ralentissement, carré)
 *   - aucune logique métier n’est implémentée ici : c’est un conteneur d’état
 */

#include "Canton.h"
#include "Config.h"
#include "debug_cc.h"

// Compteur d’instances (debug)
uint8_t CantonPeriph::comptInst = 0;

/* ============================================================================
 *  Constructeur
 * ==========================================================================*/
CantonPeriph::CantonPeriph()
    : m_id(CANTON_UNUSED_ID),
      m_busy(false),
      m_reserved(0),
      m_acces(true),
      m_locoAddr(0),
      m_masqueAigTopo(0x00),
      m_signal(0)
{
    aspectRecu[0] = 0; // AH
    aspectRecu[1] = 0; // H

    ++comptInst;
    CC_LOG_TRACE("[CantonPeriph][CC] Nouvelle instance (total=%u)\n", comptInst);
}

/* ============================================================================
 *  Destructeur
 * ==========================================================================*/
CantonPeriph::~CantonPeriph()
{
    --comptInst;
    CC_LOG_TRACE("[CantonPeriph][CC] Destruction instance (restant=%u)\n", comptInst);
}

/* ============================================================================
 *  Getters / Setters
 * ==========================================================================*/

void CantonPeriph::ID(uint8_t id)
{
    m_id = id;
    CC_LOG_TRACE("[CantonPeriph][CC] ID = %u\n", m_id);
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

void CantonPeriph::masqueAigTopo(uint8_t masqueAigTopo)
{
    m_masqueAigTopo = masqueAigTopo;
}

uint8_t CantonPeriph::masqueAigTopo()
{
    return m_masqueAigTopo;
}

/* ============================================================================
 *  Aspects reçus (H / AH)
 * ==========================================================================*/

void CantonPeriph::aspectRecuH(uint8_t asp)
{
    aspectRecu[1] = asp;
    CC_LOG_TRACE("[CantonPeriph %u][CC] aspectRecu[H] = %u\n", m_id, asp);
}

void CantonPeriph::aspectRecuAH(uint8_t asp)
{
    aspectRecu[0] = asp;
    CC_LOG_TRACE("[CantonPeriph %u][CC] aspectRecu[AH] = %u\n", m_id, asp);
}

void CantonPeriph::setAspect(SensDeMarche sens, uint8_t asp)
{
    if (sens == SensHoraire)
        aspectRecuH(asp);
    else
        aspectRecuAH(asp);
}
