/*
 * WebHandler_PilotageDistribue.cpp — Gestion Canton 2026
 */

#include "WebHandler.h"
#include "debug_cc.h"
#include "Settings.h"
#include "Canton.h"

// ---------------------------------------------------------------------------
// handlePilotageDistribue()
// ---------------------------------------------------------------------------
void WebHandler::handlePilotageDistribue(JsonDocument &doc)
{
    bool updated = false;

    // Helper générique pour uint16_t (V7)
    auto setU16 = [&](const char *key, void (*setter)(uint16_t)) {
        JsonVariant v = doc[key];
        if (!v.isNull()) {
            uint16_t val = v | 0;
            setter(val);
            CC_LOG_INFO("[PilotageDistribue][CC] %s = %u\n", key, val);
            updated = true;
        }
    };

    // Helper générique pour uint8_t (V7)
    auto setU8 = [&](const char *key, void (*setter)(uint8_t)) {
        JsonVariant v = doc[key];
        if (!v.isNull()) {
            uint8_t val = v | 0;
            setter(val);
            CC_LOG_INFO("[PilotageDistribue][CC] %s = %u\n", key, val);
            updated = true;
        }
    };

    // -----------------------------------------------------------------------
    // Longueurs (uint16_t)
    // -----------------------------------------------------------------------
    setU16("longueur_canton_mm",     Settings::setLongueurCantonMM);
    setU16("zone_ralentissement_mm", Settings::setZoneRalentissementMM);

    // -----------------------------------------------------------------------
    // Écarts (uint8_t)
    // -----------------------------------------------------------------------
    setU8("ecart_r30_n",      Settings::setEcartR30_N);
    setU8("ecart_r30_ho",     Settings::setEcartR30_HO);
    setU8("ecart_r60_n",      Settings::setEcartR60_N);
    setU8("ecart_r60_ho",     Settings::setEcartR60_HO);

    setU8("ecart_avert_n",    Settings::setEcartAvert_N);
    setU8("ecart_avert_ho",   Settings::setEcartAvert_HO);

    setU8("ecart_man_n",      Settings::setEcartMan_N);
    setU8("ecart_man_ho",     Settings::setEcartMan_HO);

    setU8("ecart_carre_n",    Settings::setEcartCarre_N);
    setU8("ecart_carre_ho",   Settings::setEcartCarre_HO);

    setU8("ecart_default_n",  Settings::setEcartDefault_N);
    setU8("ecart_default_ho", Settings::setEcartDefault_HO);

    // -----------------------------------------------------------------------
    // Rien reçu ?
    // -----------------------------------------------------------------------
    if (!updated) {
        CC_LOG_WARN("[PilotageDistribue][CC] Aucun paramètre reconnu\n");
        return;
    }

    // -----------------------------------------------------------------------
    // Sauvegarde JSON
    // -----------------------------------------------------------------------
    Settings::writeFile(canton);

    CC_LOG_INFO("[PilotageDistribue][CC] Paramètres sauvegardés\n");

    notifyClients();
}

// ---------------------------------------------------------------------------
// isPilotageDistribue() — version V7
// ---------------------------------------------------------------------------
bool WebHandler::isPilotageDistribue(JsonDocument &doc)
{
    const char *keys[] = {
        "longueur_canton_mm",
        "zone_ralentissement_mm",
        "ecart_r30_n",
        "ecart_r30_ho",
        "ecart_r60_n",
        "ecart_r60_ho",
        "ecart_avert_n",
        "ecart_avert_ho",
        "ecart_man_n",
        "ecart_man_ho",
        "ecart_carre_n",
        "ecart_carre_ho",
        "ecart_default_n",
        "ecart_default_ho"
    };

    for (const char *k : keys) {
        JsonVariant v = doc[k];
        if (!v.isNull())
            return true;
    }

    return false;
}
