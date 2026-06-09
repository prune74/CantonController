#pragma once
#include <Arduino.h>
#include "SensEnum.h" // SensHoraire / SensAntiHoraire

class Loco
{
private:
    uint16_t m_address;
    SensDeMarche m_sens; // ← plus uint8_t
    uint16_t m_speed;

public:
    Loco();

    // Adresse DCC
    void address(uint16_t address);
    uint16_t address() const;

    // Sens de marche (Exploration 2026)
    void sens(SensDeMarche sens);
    SensDeMarche sens() const;

    // Vitesse courante
    void speed(uint16_t speed);
    uint16_t speed() const;

    // Commandes rapides
    void stop();
    void ralentis(uint16_t speed);

    // Vitesses ferroviaires (configurables)
    uint16_t vitesseAvertissement;
    uint16_t vitesseRalentissement30;
    uint16_t vitesseRalentissement60;
    uint16_t vitesseRappel30;
    uint16_t vitesseRappel60;
    uint16_t vitesseVoieLibre;
    uint16_t vitesseManoeuvre;
};
/* ------------------------------------------------------------
  Fin de Loco.h
  ------------------------------------------------------------
*/
