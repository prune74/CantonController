/*
 * NodePeriph.h — Interface publique du voisin d’un canton
 * ---------------------------------------------------------------------------
 * Rôle :
 *   Représente un canton voisin dans la topologie :
 *     - SP1 / SP2 / SM1 / SM2
 *
 *   Stocke :
 *     - ID du voisin
 *     - occupation logique (busy)
 *     - autorisation d’accès (acces)
 *     - réservation (reserved)
 *     - adresse Railcom (locoAddr)
 *     - masque d’aiguilles bloquantes (masqueAig)
 *     - aspects reçus depuis EXSA (aspectRecu[2])
 */

#pragma once
#include <Arduino.h>

class NodePeriph
{
public:
    NodePeriph();
    ~NodePeriph();

    // Identité
    void ID(uint8_t id);
    uint8_t ID();

    // Occupation
    void busy(bool busy);
    bool busy();

    // Réservation
    void reserved(uint16_t locoAddr);
    uint16_t reserved();

    // Autorisation d’accès
    void acces(bool acces);
    bool acces();

    // Adresse Railcom
    void locoAddr(uint16_t addr);
    uint16_t locoAddr();

    // Masque d’aiguilles bloquantes
    void masqueAig(byte masqueAig);
    byte masqueAig();

    // Aspects reçus depuis EXSA (H / AH)
    uint8_t aspectRecu[2];

    // Debug
    static uint8_t comptInst;

private:
    uint8_t  m_id;
    bool     m_busy;
    uint16_t m_reserved;
    bool     m_acces;
    uint16_t m_locoAddr;
    byte     m_masqueAig;
    uint8_t  m_signal;
};