#include "CC_CAN_EXCC.h"
#include "Settings.h"
#include "debug_cc.h"

#include <SPIFFS.h>
#include <ArduinoJson.h>

namespace CC_CAN_EXCC
{
    void sendConfigurationSignauxDepuisSettings()
    {
        CC_LOG_INFO("[SignauxCAN][CC] Envoi configuration signaux (CONFIG_SIGNAUX)...\n");

        if (!SPIFFS.begin(true))
        {
            CC_LOG_ERROR("[SignauxCAN][CC] SPIFFS indisponible\n");
            return;
        }

        File file = SPIFFS.open("/settings.json", "r");
        if (!file)
        {
            CC_LOG_ERROR("[SignauxCAN][CC] settings.json introuvable\n");
            return;
        }

        StaticJsonDocument<1024> doc;
        if (deserializeJson(doc, file))
        {
            CC_LOG_ERROR("[SignauxCAN][CC] Erreur JSON\n");
            return;
        }

        bool sign0Absent = doc["sign0"].isNull();
        bool sign1Absent = doc["sign1"].isNull();

        uint8_t type0 = sign0Absent ? SIG_ABSENT : (uint8_t)(doc["sign0type"] | 0);
        uint8_t type1 = sign1Absent ? SIG_ABSENT : (uint8_t)(doc["sign1type"] | 0);

        uint8_t pos0 = sign0Absent ? 0 : (uint8_t)(doc["sign0position"] | 0);
        uint8_t pos1 = sign1Absent ? 0 : (uint8_t)(doc["sign1position"] | 0);

        CC_LOG_TRACE("[SignauxCAN][CC] signaux : T0=%u T1=%u P0=%u P1=%u\n",
                     type0, type1, pos0, pos1);

        sendConfigSignaux(Settings::canton, type0, type1, pos0, pos1);

        CC_LOG_INFO("[SignauxCAN][CC] Configuration signaux envoyée (CONFIG_SIGNAUX)\n");
    }
}
