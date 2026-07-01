/*
 * Settings_JSON_PilotageDistribue.cpp — Gestion Canton 2026
 */

#include "Settings.h"
#include "debug_cc.h"
#include <ArduinoJson.h>

void Settings_JSON_loadPilotageDistribue(JsonDocument &doc)
{
    // Longueur du canton
    if (doc.containsKey("longueur_canton_mm"))
        Settings::setLongueurCantonMM(doc["longueur_canton_mm"].as<uint16_t>());

    // Zone de ralentissement
    if (doc.containsKey("zone_ralentissement_mm"))
        Settings::setZoneRalentissementMM(doc["zone_ralentissement_mm"].as<uint16_t>());

    // Écarts Δ
    if (doc.containsKey("ecart_r30_n"))
        Settings::setEcartR30_N(doc["ecart_r30_n"].as<uint8_t>());
    if (doc.containsKey("ecart_r30_ho"))
        Settings::setEcartR30_HO(doc["ecart_r30_ho"].as<uint8_t>());

    if (doc.containsKey("ecart_r60_n"))
        Settings::setEcartR60_N(doc["ecart_r60_n"].as<uint8_t>());
    if (doc.containsKey("ecart_r60_ho"))
        Settings::setEcartR60_HO(doc["ecart_r60_ho"].as<uint8_t>());

    if (doc.containsKey("ecart_avert_n"))
        Settings::setEcartAvert_N(doc["ecart_avert_n"].as<uint8_t>());
    if (doc.containsKey("ecart_avert_ho"))
        Settings::setEcartAvert_HO(doc["ecart_avert_ho"].as<uint8_t>());

    if (doc.containsKey("ecart_man_n"))
        Settings::setEcartMan_N(doc["ecart_man_n"].as<uint8_t>());
    if (doc.containsKey("ecart_man_ho"))
        Settings::setEcartMan_HO(doc["ecart_man_ho"].as<uint8_t>());

    if (doc.containsKey("ecart_carre_n"))
        Settings::setEcartCarre_N(doc["ecart_carre_n"].as<uint8_t>());
    if (doc.containsKey("ecart_carre_ho"))
        Settings::setEcartCarre_HO(doc["ecart_carre_ho"].as<uint8_t>());

    if (doc.containsKey("ecart_default_n"))
        Settings::setEcartDefault_N(doc["ecart_default_n"].as<uint8_t>());
    if (doc.containsKey("ecart_default_ho"))
        Settings::setEcartDefault_HO(doc["ecart_default_ho"].as<uint8_t>());

    CC_LOG_INFO("[Settings][PilotageDistribue][CC] Paramètres chargés depuis settings.json\n");
}

void Settings_JSON_savePilotageDistribue(JsonDocument &doc)
{
    doc["longueur_canton_mm"] = Settings::longueurCantonMM();
    doc["zone_ralentissement_mm"] = Settings::zoneRalentissementMM();

    doc["ecart_r30_n"]      = Settings::ecartR30_N();
    doc["ecart_r30_ho"]     = Settings::ecartR30_HO();

    doc["ecart_r60_n"]      = Settings::ecartR60_N();
    doc["ecart_r60_ho"]     = Settings::ecartR60_HO();

    doc["ecart_avert_n"]    = Settings::ecartAvert_N();
    doc["ecart_avert_ho"]   = Settings::ecartAvert_HO();

    doc["ecart_man_n"]      = Settings::ecartMan_N();
    doc["ecart_man_ho"]     = Settings::ecartMan_HO();

    doc["ecart_carre_n"]    = Settings::ecartCarre_N();
    doc["ecart_carre_ho"]   = Settings::ecartCarre_HO();

    doc["ecart_default_n"]  = Settings::ecartDefault_N();
    doc["ecart_default_ho"] = Settings::ecartDefault_HO();

    CC_LOG_INFO("[Settings][PilotageDistribue][CC] Paramètres sauvegardés dans settings.json\n");
}
