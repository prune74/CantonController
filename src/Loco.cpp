/*
  Loco.cpp — Version 2026
  Gestion de l’état DCC de la locomotive :
  - adresse
  - sens
  - vitesse courante
  - vitesses ferroviaires configurables
*/

#include "Loco.h"

Loco::Loco()
    : m_address(0),
      m_sens(SensHoraire), // ← valeur par défaut cohérente
      m_speed(0),
      vitesseAvertissement(40),
      vitesseRalentissement30(30),
      vitesseRalentissement60(60),
      vitesseRappel30(30),
      vitesseRappel60(60),
      vitesseVoieLibre(100),
      vitesseManoeuvre(20)
{
}

/* --- Adresse DCC --- */
void Loco::address(uint16_t address) { m_address = address; }
uint16_t Loco::address() const { return m_address; }

/* --- Sens de marche (Exploration 2026) --- */
void Loco::sens(SensDeMarche sens)
{
    m_sens = sens;
}

SensDeMarche Loco::sens() const
{
    return m_sens;
}

/* --- Vitesse courante --- */
void Loco::speed(uint16_t speed) { m_speed = speed; }
uint16_t Loco::speed() const { return m_speed; }

/* --- Commandes rapides --- */
void Loco::stop() { m_speed = 0; }
void Loco::ralentis(uint16_t speed) { m_speed = speed; }
