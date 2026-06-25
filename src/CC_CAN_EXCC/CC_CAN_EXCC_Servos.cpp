#include "CC_CAN_EXCC.h"
#include "Settings.h"
#include "Aig.h"
#include "debug_cc.h"
#include <SPIFFS.h>
#include <ArduinoJson.h>

namespace CC_CAN_EXCC
{
    void sendConfigurationServosDepuisSettings()
    {
        CC_LOG_INFO("[ServoCAN][CC] Envoi configuration servos (CMD_CC_EXCC_SERVO_CONFIG)...\n");

        if (!SPIFFS.begin(true))
        {
            CC_LOG_ERROR("[ServoCAN][CC] SPIFFS indisponible\n");
            return;
        }

        File file = SPIFFS.open("/settings.json", "r");
        if (!file)
        {
            CC_LOG_ERROR("[ServoCAN][CC] settings.json introuvable\n");
            return;
        }

        StaticJsonDocument<2048> doc;
        if (deserializeJson(doc, file))
        {
            CC_LOG_ERROR("[ServoCAN][CC] Erreur JSON servos\n");
            return;
        }

        for (uint8_t servo = 0; servo < aigSize; ++servo)
        {
            Aig *aig = Settings::canton->getAig(servo);
            if (!aig)
                continue;

            char keyPosDroit[16];
            char keyPosDevie[16];
            char keySpeed[16];

            snprintf(keyPosDroit, sizeof(keyPosDroit), "aig%uposDroit", servo);
            snprintf(keyPosDevie, sizeof(keyPosDevie), "aig%uposDevie", servo);
            snprintf(keySpeed, sizeof(keySpeed), "aig%uspeed", servo);

            uint16_t posDroit = doc[keyPosDroit] | aig->posDroit();
            uint16_t posDevie = doc[keyPosDevie] | aig->posDevie();
            uint16_t speedSlider = doc[keySpeed] | 5;
            uint16_t speed = 11000 - (speedSlider * 1000);

            sendServoConfig(Settings::canton, servo, posDroit, posDevie, speed);

            CC_LOG_INFO("[ServoCAN][CC] CMD_CC_EXCC_SERVO_CONFIG → servo %u (D:%u V:%u S:%u)\n",
                        servo, posDroit, posDevie, speed);
        }

        CC_LOG_INFO("[ServoCAN][CC] Configuration servos envoyée\n");
    }
}