/*
  Aig.h — Version 2026 (CLEAN)
  ------------------------------------------------------------
  Représente une AIGUILLE LOGIQUE du réseau.

  Rôle :
  - Le SA ne pilote plus physiquement les servos.
  - EXSA (H ou AH) pilote les servos via PCA9685.
  - Cette classe stocke uniquement :
        * position droite / déviée (µs)
        * position logique courante
        * index EXSA côté H (SP1)
        * index EXSA côté AH (SM1)
  - Le SA envoie ces paramètres à EXSA via RS485 (F0/F1/F2).

  Aucun PWM local n’est utilisé dans l’architecture 2026.
  ------------------------------------------------------------
*/

#pragma once
#include <Arduino.h>
#include "Config.h"

class Aig
{
protected:
  // Identifiant logique de l’aiguille (0..N)
  uint8_t m_id;

  // Positions logiques droite / déviée (µs)
  uint16_t m_posDroit;
  uint16_t m_posDevie;

  // Limites mécaniques (validation des valeurs)
  uint16_t m_minPos;
  uint16_t m_maxPos;

  // État logique actuel (true = droite)
  bool m_estDroit;

  // Position logique courante (EXSA fera le mouvement réel)
  uint16_t m_curPos;

  // Index EXSA côté H (SP1)
  uint8_t m_cantonPdroitIdx;

  // Index EXSA côté AH (SM1)
  uint8_t m_cantonPdevieIdx;

public:
  Aig();
  ~Aig();

  // Identifiant logique
  void ID(uint8_t);
  uint8_t ID() const;

  // État logique (droite / déviée)
  void estDroit(bool);
  bool estDroit() const;

  // Positions logiques droite / déviée
  void posDroit(uint16_t);
  void posDevie(uint16_t);
  uint16_t posDroit() const;
  uint16_t posDevie() const;

  // Position logique courante
  void curPos(uint16_t);
  uint16_t curPos() const;

  // Index EXSA côté H (SP1)
  void cantonPdroitIdx(uint8_t);
  uint8_t cantonPdroitIdx() const;

  // Index EXSA côté AH (SM1)
  void cantonPdevieIdx(uint8_t);
  uint8_t cantonPdevieIdx() const;
};
/* ------------------------------------------------------------
  Fin de Aig.h
  ------------------------------------------------------------
*/