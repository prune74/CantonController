/*
 * Settings_JSON_PilotageDistribue.cpp — Gestion Canton 2026
 */

#include "Settings.h"
#include "debug_cc.h"
#include <ArduinoJson.h>

void Settings_JSON_loadPilotageDistribue(JsonDocument &doc)
{
    // Longueur du canton
    {
        JsonVariant v = doc["longueur_canton_mm"];
        if (!v.isNull())
            Settings::setLongueurCantonMM(v | Settings::longueurCantonMM());
    }

    // Zone de ralentissement
    {
        JsonVariant v = doc["zone_ralentissement_mm"];
        if (!v.isNull())
            Settings::setZoneRalentissementMM(v | Settings::zoneRalentissementMM());
    }

    // Écarts Δ
    {
        JsonVariant v = doc["ecart_r30_n"];
        if (!v.isNull())
            Settings::setEcartR30_N(v | Settings::ecartR30_N());
    }
    {
        JsonVariant v = doc["ecart_r30_ho"];
        if (!v.isNull())
            Settings::setEcartR30_HO(v | Settings::ecartR30_HO());
    }

    {
        JsonVariant v = doc["ecart_r60_n"];
        if (!v.isNull())
            Settings::setEcartR60_N(v | Settings::ecartR60_N());
    }
    {
        JsonVariant v = doc["ecart_r60_ho"];
        if (!v.isNull())
            Settings::setEcartR60_HO(v | Settings::ecartR60_HO());
    }

    {
        JsonVariant v = doc["ecart_avert_n"];
        if (!v.isNull())
            Settings::setEcartAvert_N(v | Settings::ecartAvert_N());
    }
    {
        JsonVariant v = doc["ecart_avert_ho"];
        if (!v.isNull())
            Settings::setEcartAvert_HO(v | Settings::ecartAvert_HO());
    }

    {
        JsonVariant v = doc["ecart_man_n"];
        if (!v.isNull())
            Settings::setEcartMan_N(v | Settings::ecartMan_N());
    }
    {
        JsonVariant v = doc["ecart_man_ho"];
        if (!v.isNull())
            Settings::setEcartMan_HO(v | Settings::ecartMan_HO());
    }

    {
        JsonVariant v = doc["ecart_carre_n"];
        if (!v.isNull())
            Settings::setEcartCarre_N(v | Settings::ecartCarre_N());
    }
    {
        JsonVariant v = doc["ecart_carre_ho"];
        if (!v.isNull())
            Settings::setEcartCarre_HO(v | Settings::ecartCarre_HO());
    }

    {
        JsonVariant v = doc["ecart_default_n"];
        if (!v.isNull())
            Settings::setEcartDefault_N(v | Settings::ecartDefault_N());
    }
    {
        JsonVariant v = doc["ecart_default_ho"];
        if (!v.isNull())
            Settings::setEcartDefault_HO(v | Settings::ecartDefault_HO());
    }

    CC_LOG_INFO("[Settings][PilotageDistribue][CC] Paramètres chargés depuis settings.json\n");
}

void Settings_JSON_savePilotageDistribue(JsonDocument &doc)
{
    doc["longueur_canton_mm"]     = Settings::longueurCantonMM();
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
