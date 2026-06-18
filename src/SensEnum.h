/*
 * SensEnum.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Définition du sens de marche ferroviaire utilisé par le Canton Controller.
 *
 * Ce fichier ne redéfinit PAS les capteurs H / AH.
 * Il fournit uniquement l’énumération utilisée par :
 *   - Loco::sens()
 *   - SensRoulage.cpp
 *   - Sensor.cpp (callbacks EXCC)
 *   - SupervisionCanton.cpp
 *
 * Valeurs :
 *   SensHoraire      → déplacement vers SP1
 *   SensAntiHoraire  → déplacement vers SM1
 */

#pragma once
#include <stdint.h>

enum SensDeMarche : uint8_t
{
    SensHoraire      = 1,
    SensAntiHoraire  = 2
};
