/*
 * Booster.cpp — Gestion du Booster
 * ---------------------------------------------------------------------------
 * Ce module stocke les informations remontées par l’EXCC :
 *   - tension
 *   - courant
 *   - état interne
 *   - seuils de calibration (libre / occupé)
 *
 * Il ne contient aucune logique métier :
 *   → il met simplement à jour les valeurs internes et Settings.
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

uint16_t Booster::s_seuilLibre   = 0;
uint16_t Booster::s_seuilOccupe  = 0;

// ---------------------------------------------------------------------------
// Mise à jour Booster (tension / courant / état)
// ---------------------------------------------------------------------------
void Booster::onBooster(uint8_t tension,
                        uint8_t courant,
                        uint8_t etat)
{
    s_tension = tension;
    s_courant = courant;
    s_etat    = etat;

    CC_LOG_INFO("[Booster][CC] U=%u  I=%u  etat=%u\n",
                tension, courant, etat);
}

// ---------------------------------------------------------------------------
// Calibration Booster (seuils libre / occupé)
// ---------------------------------------------------------------------------
void Booster::onCalib(uint8_t libre_L,
                      uint8_t libre_H,
                      uint8_t occupe_L,
                      uint8_t occupe_H)
{
    uint16_t libre  = (uint16_t(libre_H)  << 8) | libre_L;
    uint16_t occupe = (uint16_t(occupe_H) << 8) | occupe_L;

    s_seuilLibre  = libre;
    s_seuilOccupe = occupe;

    CC_LOG_INFO("[Booster][CC] Calib → libre=%u  occupé=%u\n",
                libre, occupe);

    // Mise à jour Settings
    Settings::setBoosterSeuilLibre(libre);
    Settings::setBoosterSeuilOccupe(occupe);

    // Sauvegarde JSON
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
