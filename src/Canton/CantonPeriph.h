/*
 * CantonPeriph.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Représentation d’un canton voisin dans la topologie locale :
 *
 *     SP1 / SP2 / SM1 / SM2
 *
 * Un CantonPeriph stocke l’état d’un voisin :
 *   - ID du voisin
 *   - occupation logique (busy)
 *   - réservation (reserved)
 *   - autorisation d’accès (acces)
 *   - adresse RailCom de la loco présente (locoAddr)
 *   - masque d’aiguilles bloquantes (masqueAig)
 *   - aspects reçus depuis EXCC (aspectRecu[H/AH])
 *
 * IMPORTANT :
 *   - aucune logique métier ici
 *   - aucune décision ferroviaire
 *   - uniquement un conteneur d’état pour la topologie
 */

#pragma once
#include <Arduino.h>
#include "FeuxDirection/FeuxDirection_Types.h" // pour SensDeMarche

class CantonPeriph
{
public:
    CantonPeriph();
    ~CantonPeriph();

    /* ------------------------------------------------------------------------
     * Identité
     * ------------------------------------------------------------------------ */
    void ID(uint8_t id);
    uint8_t ID();

    /* ------------------------------------------------------------------------
     * Occupation logique
     * ------------------------------------------------------------------------ */
    void busy(bool busy);
    bool busy();

    /* ------------------------------------------------------------------------
     * Réservation (adresse DCC de la loco)
     * ------------------------------------------------------------------------ */
    void reserved(uint16_t locoAddr);
    uint16_t reserved();

    /* ------------------------------------------------------------------------
     * Autorisation d’accès
     * ------------------------------------------------------------------------ */
    void acces(bool acces);
    bool acces();

    /* ------------------------------------------------------------------------
     * Adresse RailCom de la loco présente
     * ------------------------------------------------------------------------ */
    void locoAddr(uint16_t addr);
    uint16_t locoAddr();

    /* ------------------------------------------------------------------------
     * Masque d’aiguilles bloquantes
     * ------------------------------------------------------------------------ */
    void masqueAig(uint8_t masqueAig);
    uint8_t masqueAig();

    /* ------------------------------------------------------------------------
     * Aspects reçus depuis EXCC (H / AH)
     * ------------------------------------------------------------------------ */
    void aspectRecuH(uint8_t asp);  // sens horaire
    void aspectRecuAH(uint8_t asp); // sens antihoraire
    void setAspect(SensDeMarche sens, uint8_t asp);

    uint8_t aspectRecu[2]; // [0]=AH, [1]=H

    /* ------------------------------------------------------------------------
     * Debug : compteur d’instances
     * ------------------------------------------------------------------------ */
    static uint8_t comptInst;

private:
    uint8_t m_id;
    bool m_busy;
    uint16_t m_reserved;
    bool m_acces;
    uint16_t m_locoAddr;
    uint8_t m_masqueAig;
    uint8_t m_signal; // réservé pour extensions futures
};
