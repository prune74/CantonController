#include "CC_CAN_EXCC.h"
#include "Settings.h"
#include "debug_cc.h"

#include <SPIFFS.h>
#include <ArduinoJson.h>

namespace CC_CAN_EXCC
{

    /* ============================================================================
     *  Envoi du profil de voie depuis settings.json → EXCC
     *  (appelé lors de EXCC_Link::onExccOnline)
     * ==========================================================================*/
    void sendTrackProfileDepuisSettings()
    {
        CC_LOG_INFO("[TrackProfileCAN][CC] Lecture profil dans settings.json...\n");

        if (!SPIFFS.begin(true))
        {
            CC_LOG_ERROR("[TrackProfileCAN][CC] SPIFFS indisponible\n");
            return;
        }

        File file = SPIFFS.open("/settings.json", "r");
        if (!file)
        {
            CC_LOG_ERROR("[TrackProfileCAN][CC] settings.json introuvable\n");
            return;
        }

        StaticJsonDocument<512> doc;
        if (deserializeJson(doc, file))
        {
            CC_LOG_ERROR("[TrackProfileCAN][CC] Erreur JSON\n");
            return;
        }

        bool profile = doc["track_profile"] | false;

        CC_LOG_INFO("[TrackProfileCAN][CC] Profil lu : %s",
                    profile ? "15V (HO)" : "12V (N)");

        // Envoi au EXCC
        sendTrackProfile(Settings::canton, profile);

        CC_LOG_INFO("[TrackProfileCAN][CC] Profil envoyé (SET_PROFILE)\n");
    }

    /* ============================================================================
     *  Envoi direct du profil de voie (booléen) vers EXCC
     * ==========================================================================*/
    void sendTrackProfile(Canton *canton, bool profile)
    {
        uint8_t payload[1] = {uint8_t(profile ? 1 : 0)};

        sendEXCC(
            1,
            (uint16_t)Cmd_CC_to_EXCC::PROFILE_VOIE,
            0,
            canton->ID(),
            payload,
            1);

        CC_LOG_INFO("[TrackProfileCAN][CC] PROFILE_VOIE envoyé : %s",
                    profile ? "15V (HO)" : "12V (N)");
    }
}
