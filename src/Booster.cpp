#include "Booster.h"
#include "debug_sa.h"
#include "Settings.h"

uint8_t  Booster::s_tension  = 0;
uint8_t  Booster::s_courant  = 0;
uint8_t  Booster::s_etat     = 0;
uint8_t  Booster::s_present  = 0;

uint16_t Booster::s_seuilLibre  = 0;
uint16_t Booster::s_seuilOccupe = 0;

void Booster::onBooster(uint8_t index_exsa,
                        uint8_t tension,
                        uint8_t courant,
                        uint8_t etat,
                        uint8_t present)
{
    s_tension = tension;
    s_courant = courant;
    s_etat    = etat;
    s_present = present;

    SA_LOG_INFO("[Booster] EXSA %u → U=%u  I=%u  etat=%u  present=%u\n",
                index_exsa, tension, courant, etat, present);
}

void Booster::onCalib(uint8_t index_exsa,
                      uint8_t libre_L,
                      uint8_t libre_H,
                      uint8_t occupe_L,
                      uint8_t occupe_H)
{
    uint16_t libre  = (uint16_t(libre_H)  << 8) | libre_L;
    uint16_t occupe = (uint16_t(occupe_H) << 8) | occupe_L;

    s_seuilLibre  = libre;
    s_seuilOccupe = occupe;

    SA_LOG_INFO("[Booster] Calib EXSA=%u → libre=%u  occupe=%u\n",
                index_exsa, libre, occupe);

    Settings::setBoosterSeuilLibre(libre);
    Settings::setBoosterSeuilOccupe(occupe);
    Settings::save();
}

void Booster::setSeuils(uint16_t libre, uint16_t occupe)
{
    s_seuilLibre  = libre;
    s_seuilOccupe = occupe;

    SA_LOG_INFO("[Booster] Seuils chargés → libre=%u occupe=%u\n",
                libre, occupe);
}
