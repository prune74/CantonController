#pragma once
#include <stdint.h>

/*
 * Booster.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Gestion des informations Booster remontées par l’EXCC :
 *   - tension (U)
 *   - courant (I)
 *   - état (flags internes)
 *   - seuils de calibration (libre / occupé)
 *
 * Ce module ne contient aucune logique métier :
 *   → il stocke les valeurs reçues et met à jour Settings.
 */

class Booster
{
public:
    // Mise à jour des valeurs instantanées
    static void onBooster(uint8_t tension,
                          uint8_t courant,
                          uint8_t etat);

    // Calibration (seuils libre / occupé)
    static void onCalib(uint8_t libre_L,
                        uint8_t libre_H,
                        uint8_t occupe_L,
                        uint8_t occupe_H);

    // Chargement manuel des seuils
    static void setSeuils(uint16_t libre, uint16_t occupe);

    // Accès aux valeurs
    static inline uint8_t  tension()     { return s_tension; }
    static inline uint8_t  courant()     { return s_courant; }
    static inline uint8_t  etat()        { return s_etat; }
    static inline uint16_t seuilLibre()  { return s_seuilLibre; }
    static inline uint16_t seuilOccupe() { return s_seuilOccupe; }

private:
    static uint8_t  s_tension;
    static uint8_t  s_courant;
    static uint8_t  s_etat;

    static uint16_t s_seuilLibre;
    static uint16_t s_seuilOccupe;
};
