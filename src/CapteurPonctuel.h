#pragma once
#include <stdint.h>

class CapteurPonctuel
{
public:
    static void onPonctuelH(uint8_t etat);
    static void onPonctuelAH(uint8_t etat);
};
