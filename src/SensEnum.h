#pragma once

/*
 * SensEnum.h — Version Exploration 2026
 * ---------------------------------------------------------------------------
 * Ce fichier ne redéfinit PLUS horaire / antiHor.
 * Il fournit uniquement un alias propre si nécessaire.
 */

enum SensDeMarche : uint8_t
{
    SensHoraire = 1,
    SensAntiHoraire = 2
};