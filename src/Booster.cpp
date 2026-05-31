#include "Booster.h"
#include "debug_sa.h"
#include "Settings.h"        // Pour sauvegarde JSON

/* ============================================================
   Variables statiques
   ============================================================ */
uint8_t  Booster::s_voltage = 0;
uint8_t  Booster::s_current = 0;
uint8_t  Booster::s_flags   = 0;
uint8_t  Booster::s_temp    = 0;

uint16_t Booster::s_seuilLibre  = 0;
uint16_t Booster::s_seuilOccupe = 0;

/* ============================================================
   PROTO_07 — État booster (EXSA → SA)
   ============================================================ */
void Booster::onBooster(uint8_t index_exsa,
                        uint8_t voltage,
                        uint8_t current,
                        uint8_t flags,
                        uint8_t temp)
{
    s_voltage = voltage;
    s_current = current;
    s_flags   = flags;
    s_temp    = temp;

    SA_LOG_INFO("[Booster] EXSA %u → U=%uV  I=%uA  flags=0x%02X  T=%u°C\n",
                index_exsa, voltage, current, flags, temp);
}

/* ============================================================
   PROTO_09 — Seuils calibrés (EXSA → SA)
   ============================================================ */
void Booster::onCalib(uint8_t index_exsa,
                      uint8_t libre_L,
                      uint8_t libre_H,
                      uint8_t occupe_L,
                      uint8_t occupe_H)
{
    uint16_t libre  = uint16_t(libre_H  << 8) | libre_L;
    uint16_t occupe = uint16_t(occupe_H << 8) | occupe_L;

    s_seuilLibre  = libre;
    s_seuilOccupe = occupe;

    SA_LOG_INFO("[Booster] Calib EXSA=%u → libre=%u  occupe=%u\n",
                index_exsa, libre, occupe);

    // Sauvegarde dans settings.json
    Settings::setBoosterSeuilLibre(libre);
    Settings::setBoosterSeuilOccupe(occupe);
    Settings::save();
}

/* ============================================================
   Chargement des seuils depuis settings.json au démarrage
   ============================================================ */
void Booster::setSeuils(uint16_t libre, uint16_t occupe)
{
    s_seuilLibre  = libre;
    s_seuilOccupe = occupe;

    SA_LOG_INFO("[Booster] Seuils chargés depuis settings.json → libre=%u occupe=%u\n",
                libre, occupe);
}
