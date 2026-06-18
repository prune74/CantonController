/*
 * Sensor.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Représentation d’un capteur ponctuel virtuel pour le Canton Controller (CC).
 *
 * Les capteurs physiques sont lus par l’Extension Canton Controller (EXCC),
 * qui envoie des notifications PROTO_PONCT_H / PROTO_PONCT_AH.
 *
 * Ce module :
 *   - stocke l’état interne du capteur (actif / libre)
 *   - permet de forcer ou surcharger cet état
 *   - fournit les callbacks statiques appelés lors des trames EXCC
 *
 * Aucun GPIO n’est utilisé localement : les paramètres de setup() sont
 * conservés uniquement pour compatibilité API.
 */

#pragma once

#include <stdint.h>
#include <driver/gpio.h>
#include <Arduino.h>

class Sensor
{
public:
    Sensor();
    ~Sensor();

    // Capteur virtuel : paramètres conservés mais inutilisés
    void setup(gpio_num_t pin, uint32_t tempo, uint8_t input);

    // Lecture / écriture interne
    bool state();
    void state(bool state);

    // Mise à jour interne (utilisé par le CC)
    void overrideState(bool state);

    // Callbacks EXCC (PROTO_03_H / PROTO_03_AH)
    static void onPonctuelH(uint8_t code);
    static void onPonctuelAH(uint8_t code);

private:
    bool m_state;  // true = ACTIF, false = LIBRE
};
