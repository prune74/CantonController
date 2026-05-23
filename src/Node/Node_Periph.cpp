/*
 * Node_Periph.cpp — Gestion des voisins d’un canton (NodePeriph)
 * ---------------------------------------------------------------------------
 * Rôle :
 *   NodePeriph représente un "voisin" dans la topologie ferroviaire :
 *
 *       SP1 / SP2 / SM1 / SM2
 *
 *   Chaque NodePeriph correspond à un canton adjacent et stocke :
 *     - son ID
 *     - son occupation logique (busy)
 *     - son autorisation d’accès (acces)
 *     - la loco réservée dans ce voisin (reserved)
 *     - l’adresse Railcom de la loco présente (locoAddr)
 *     - les aiguilles bloquantes pour accéder à ce voisin (masqueAig)
 *     - les aspects reçus depuis EXSA (aspectRecu[2])
 *
 *   Ce module est volontairement séparé pour :
 *     - clarifier la topologie
 *     - simplifier Node.cpp
 *     - faciliter l’onboarding Discovery 2026
 */

#include "Node_Internal.h"

// Compteur d’instances (debug)
uint8_t NodePeriph::comptInst = 0;

/*
 * Constructeur
 * ---------------------------------------------------------------------------
 * Initialise toutes les valeurs internes à un état neutre.
 */
NodePeriph::NodePeriph()
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

    SA_LOG_TRACE("[NodePeriph] Nouvelle instance (total=%u)\n", comptInst);
}

/*
 * Destructeur
 * ---------------------------------------------------------------------------
 */
NodePeriph::~NodePeriph()
{
    --comptInst;
    SA_LOG_TRACE("[NodePeriph] Destruction instance (restant=%u)\n", comptInst);
}

/* ============================================================================
 * Getters / Setters
 * ---------------------------------------------------------------------------
 * Tous les accès sont simples et directs.
 * Aucun traitement métier ici : ce fichier est purement "données".
 * ============================================================================
 */

void NodePeriph::ID(uint8_t id)
{
    m_id = id;
    SA_LOG_TRACE("[NodePeriph] ID défini = %u\n", m_id);
}

uint8_t NodePeriph::ID()
{
    return m_id;
}

void NodePeriph::busy(bool busy)
{
    m_busy = busy;
}

bool NodePeriph::busy()
{
    return m_busy;
}

void NodePeriph::reserved(uint16_t locoAddr)
{
    m_reserved = locoAddr;
}

uint16_t NodePeriph::reserved()
{
    return m_reserved;
}

void NodePeriph::acces(bool acces)
{
    m_acces = acces;
}

bool NodePeriph::acces()
{
    return m_acces;
}

void NodePeriph::locoAddr(uint16_t addr)
{
    m_locoAddr = addr;
}

uint16_t NodePeriph::locoAddr()
{
    return m_locoAddr;
}

void NodePeriph::masqueAig(byte masqueAig)
{
    m_masqueAig = masqueAig;
}

byte NodePeriph::masqueAig()
{
    return m_masqueAig;
}
