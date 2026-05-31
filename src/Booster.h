#pragma once
#include <stdint.h>

class Booster
{
public:
    /* ============================================================
       PROTO_07 — État booster (EXSA → SA)
       ============================================================ */
    static void onBooster(uint8_t index_exsa,
                          uint8_t voltage,
                          uint8_t current,
                          uint8_t flags,
                          uint8_t temp);

    /* ============================================================
       PROTO_09 — Seuils calibrés (EXSA → SA)
       ============================================================ */
    static void onCalib(uint8_t index_exsa,
                        uint8_t libre_L,
                        uint8_t libre_H,
                        uint8_t occupe_L,
                        uint8_t occupe_H);

    /* ============================================================
       Accès aux valeurs stockées
       ============================================================ */
    static uint8_t  voltage() { return s_voltage; }
    static uint8_t  current() { return s_current; }
    static uint8_t  flags()   { return s_flags; }
    static uint8_t  temp()    { return s_temp; }

    static uint16_t seuilLibre()  { return s_seuilLibre; }
    static uint16_t seuilOccupe() { return s_seuilOccupe; }

    /* ============================================================
       Mise à jour depuis settings.json au démarrage
       ============================================================ */
    static void setSeuils(uint16_t libre, uint16_t occupe);

private:
    /* État temps réel (PROTO_07) */
    static uint8_t s_voltage;
    static uint8_t s_current;
    static uint8_t s_flags;
    static uint8_t s_temp;

    /* Seuils calibrés (PROTO_09) */
    static uint16_t s_seuilLibre;
    static uint16_t s_seuilOccupe;
};
