#pragma once
#include <stdint.h>

class Booster
{
public:
    // PROTO_07 — État booster (EXSA → SA)
    static void onBooster(uint8_t index_exsa,
                          uint8_t tension,
                          uint8_t courant,
                          uint8_t etat,
                          uint8_t present);

    // PROTO_09 — Seuils calibrés (EXSA → SA)
    static void onCalib(uint8_t index_exsa,
                        uint8_t libre_L,
                        uint8_t libre_H,
                        uint8_t occupe_L,
                        uint8_t occupe_H);

    // Accès aux valeurs stockées
    static uint8_t  tension() { return s_tension; }
    static uint8_t  courant() { return s_courant; }
    static uint8_t  etat()    { return s_etat; }
    static uint8_t  present() { return s_present; }

    static uint16_t seuilLibre()  { return s_seuilLibre; }
    static uint16_t seuilOccupe() { return s_seuilOccupe; }

    // Mise à jour depuis settings.json au démarrage
    static void setSeuils(uint16_t libre, uint16_t occupe);

private:
    // État temps réel (PROTO_07)
    static uint8_t s_tension;
    static uint8_t s_courant;
    static uint8_t s_etat;
    static uint8_t s_present;

    // Seuils calibrés (PROTO_09)
    static uint16_t s_seuilLibre;
    static uint16_t s_seuilOccupe;
};
