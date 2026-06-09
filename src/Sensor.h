#pragma once
#include <stdint.h>
#include <driver/gpio.h>

#include <Arduino.h>

class Sensor
{
public:
    Sensor();
    ~Sensor();

    // Version 2026 : paramètres conservés mais inutilisés
    void setup(gpio_num_t pin, uint32_t tempo, uint8_t input);

    bool state();
    void state(bool state);

    void overrideState(bool state);

    // Callback Exploration 2026 (PROTO_03)
    static void onPonctuel(uint8_t index_exsa, uint8_t code);

private:
    bool m_state;
};
