#pragma once
#include <stdint.h>

class Railcom
{
public:
    static void begin();

    // Adresse RailCom stabilisée reçue d’EXSA
    static uint16_t address();

    // Callback appelé par SA_UartRx
    static void onRailcom(uint8_t index_exsa, uint8_t low, uint8_t high);

private:
    static volatile uint16_t s_address;
};
