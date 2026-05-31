#pragma once
#include <stdint.h>

class SatEXSA_Link
{
public:
    static void begin();
    static void loop();

    /* Supervision EXSA */
    static bool  isOnline(uint8_t index);
    static void  onPong(uint8_t index);
    static void  onExsaOnline(uint8_t index);
    static void  onExsaOffline(uint8_t index);

    /* Booster */
    static void onBooster(uint8_t index,
                          uint8_t etat,
                          uint8_t courant,
                          uint8_t tension,
                          uint8_t present);

    static int8_t getBoosterExsaIndex();

    /* Commandes SA → EXSA */
    static void envoyerBoosterPower(uint8_t index, bool on);
    static void demanderRecalibration(uint8_t index);                 // F3
    static void envoyerSeuilsBooster(uint8_t index,
                                     uint16_t libre,
                                     uint16_t occupe);                // F4

private:
    static void envoyerPingPeriodique();
};
