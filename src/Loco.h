/*
 * Loco.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Représentation interne d’une locomotive dans le Canton Controller (CC).
 *
 * Ce module stocke :
 *   - l’adresse DCC (RailCom ou configuration)
 *   - le sens de marche (SensHoraire / SensAntiHoraire)
 *   - la vitesse courante (0..127)
 *   - les vitesses ferroviaires configurables :
 *        • avertissement
 *        • ralentissement 30 / 60
 *        • rappel 30 / 60
 *        • voie libre
 *        • manœuvre
 *
 * Aucun calcul métier n’est effectué ici : la classe Loco est un simple
 * conteneur d’état utilisé par SupervisionCanton, PilotageDistribue,
 * GestionReseau et Railcom.
 */

#pragma once

#include <Arduino.h>
#include "SensEnum.h"   // SensHoraire / SensAntiHoraire

class Loco
{
public:
    Loco();

    // -----------------------------------------------------------------------
    // Adresse DCC
    // -----------------------------------------------------------------------
    void address(uint16_t address);
    uint16_t address() const;

    // -----------------------------------------------------------------------
    // Sens de marche
    // -----------------------------------------------------------------------
    void sens(SensDeMarche sens);
    SensDeMarche sens() const;

    // -----------------------------------------------------------------------
    // Vitesse courante
    // -----------------------------------------------------------------------
    void speed(uint16_t speed);
    uint16_t speed() const;

    // -----------------------------------------------------------------------
    // Commandes rapides
    // -----------------------------------------------------------------------
    void stop();
    void ralentis(uint16_t speed);

    // -----------------------------------------------------------------------
    // Vitesses ferroviaires configurables
    // -----------------------------------------------------------------------
    uint16_t vitesseAvertissement;
    uint16_t vitesseRalentissement30;
    uint16_t vitesseRalentissement60;
    uint16_t vitesseRappel30;
    uint16_t vitesseRappel60;
    uint16_t vitesseVoieLibre;
    uint16_t vitesseManoeuvre;

private:
    uint16_t m_address;        // Adresse DCC
    SensDeMarche m_sens;       // SensHoraire / SensAntiHoraire
    uint16_t m_speed;          // Vitesse courante
};
