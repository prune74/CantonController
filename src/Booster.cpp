/*
 * Booster.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Gestion des informations Booster remontées par l’EXCC :
 *   - tension
 *   - courant
 *   - état (flags internes)
 *   - présence
 *   - seuils de calibration (libre / occupé)
 *
 * Ce module ne contient aucune logique métier :
 *   → il stocke les valeurs reçues et met à jour Settings.
 */

#include "Booster.h"
#include "debug_cc.h"
#include "Settings.h"

// ---------------------------------------------------------------------------
// Variables statiques
// ---------------------------------------------------------------------------
uint8_t  Booster::s_tension      = 0;
uint8_t  Booster::s_courant      = 0;
uint8_t  Booster::s_etat         = 0;
uint8_t  Booster::s_present      = 0;

uint16_t Booster::s_seuilLibre   = 0;
uint16_t Booster::s_seuilOccupe  = 0;

// ---------------------------------------------------------------------------
// Mise à jour Booster (tension / courant / état / présence)
// ---------------------------------------------------------------------------
void Booster::onBooster(uint8_t index_excc,
                        uint8_t tension,
                        uint8_t courant,
                        uint8_t etat,
                        uint8_t present)
{
    s_tension = tension;
    s_courant = courant;
    s_etat    = etat;
    s_present = present;

    CC_LOG_INFO("[Booster][CC] EXCC %u → U=%u  I=%u  etat=%u  present=%u\n",
                index_excc, tension, courant, etat, present);
}

// ---------------------------------------------------------------------------
// Calibration Booster (seuils libre / occupé)
// ---------------------------------------------------------------------------
void Booster::onCalib(uint8_t index_excc,
                      uint8_t libre_L,
                      uint8_t libre_H,
                      uint8_t occupe_L,
                      uint8_t occupe_H)
{
    uint16_t libre  = (uint16_t(libre_H)  << 8) | libre_L;
    uint16_t occupe = (uint16_t(occupe_H) << 8) | occupe_L;

    s_seuilLibre  = libre;
    s_seuilOccupe = occupe;

    CC_LOG_INFO("[Booster][CC] Calib EXCC=%u → libre=%u  occupé=%u\n",
                index_excc, libre, occupe);

    // Mise à jour interne Settings
    Settings::setBoosterSeuilLibre(libre);
    Settings::setBoosterSeuilOccupe(occupe);

    // Sauvegarde JSON 2026
    Settings::writeFile(Settings::canton);
}

// ---------------------------------------------------------------------------
// Chargement manuel des seuils (depuis Settings)
// ---------------------------------------------------------------------------
void Booster::setSeuils(uint16_t libre, uint16_t occupe)
{
    s_seuilLibre  = libre;
    s_seuilOccupe = occupe;

    CC_LOG_INFO("[Booster][CC] Seuils chargés → libre=%u  occupé=%u\n",
                libre, occupe);
}
