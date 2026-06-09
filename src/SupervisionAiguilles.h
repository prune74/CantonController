#pragma once
#include <stdint.h>

class Canton;

class SupervisionAiguilles
{
public:
    static void begin(Canton *canton);

    static void onPosition(uint8_t index_exsa,
                           uint8_t idAig,
                           uint8_t etat,
                           uint8_t masque);

private:
    static Canton *s_canton;
};
