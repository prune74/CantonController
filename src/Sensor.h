#pragma once

#include <Arduino.h>

/**
 * @brief Classe Sensor utilisée côté Satellite Autonome
 *        pour superviser l’état d’un capteur ponctuel.
 *        Version 2026 : plus de capteur physique → 100% UART.
 */
class Sensor {
public:
  Sensor();
  ~Sensor();

  /**
   * @brief Initialise le capteur
   *        (paramètres conservés pour compatibilité API,
   *         mais ignorés en version 2026)
   */
  void setup(gpio_num_t pin = GPIO_NUM_NC,
             uint32_t tempo = 0,
             byte input = INPUT);

  bool state();                     // Lecture de l’état actuel
  void state(bool state);           // Écriture interne
  void overrideState(bool state);   // Mise à jour via trame UART

  static void IRAM_ATTR uartLoop(void* p); // Réception UART (EXSA → SA)

private:
  bool m_state;                     // État logique du capteur
};
