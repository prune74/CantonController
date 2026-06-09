/*
 * SatTopo_UART_Servos.cpp
 * ------------------------------------------------------------
 * Gestion des servos EXSA :
 *
 *   - F0 : Mouvement réel (servoMove)
 *   - F1 : Configuration servo (posDroit / posDevie / speed)
 *   - F2 : Test servo
 *
 * Ce module transmet à EXSA les paramètres physiques des servos
 * définis dans settings.json.
 *
 * IMPORTANT :
 *   - La logique métier (estDroit, posDroit, posDevie, etc.)
 *     est gérée dans Aig.cpp et Canton_Aiguilles.cpp.
 *
 *   - Ce module ne fait que traduire les paramètres Exploration 2026
 *     (slider 0–10) en valeurs EXSA (11000 → 1000).
 */

#include "SatTopologieUART.h"
#include "Config.h"
#include "Exploration_Protocol.h"
#include "debug_sa.h"

#include "Settings.h"
#include "Aig.h"
#include "Canton.h"

#include <SPIFFS.h>
#include <FS.h>
#include <ArduinoJson.h>

extern HardwareSerial Serial1;

/*-------------------------------------------------------------
  Commande F0 : mouvement réel du servo
--------------------------------------------------------------*/
void envoyerServoMove(uint8_t exsaAdresse, uint8_t servoIndex)
{
  SA_LOG_INFO("[ServoUART] Move : exsa=%u servo=%u\n", exsaAdresse, servoIndex);

  Serial1.write(PROTO_SYNC_BYTE);
  Serial1.write(PROTO_F0_SERVO_MOVE);
  Serial1.write(exsaAdresse);
  Serial1.write(servoIndex);
}

/*-------------------------------------------------------------
  Envoie la configuration des servos depuis settings.json
  → opcode F1
--------------------------------------------------------------*/
void envoyerConfigurationServosDepuisSettings()
{
  SA_LOG_INFO("[ServoUART] Envoi configuration servos (F1)...\n");

  if (!SPIFFS.begin(true))
  {
    SA_LOG_ERROR("[ServoUART] SPIFFS indisponible\n");
    return;
  }

  File file = SPIFFS.open("/settings.json", "r");
  if (!file)
  {
    SA_LOG_ERROR("[ServoUART] settings.json introuvable\n");
    return;
  }

  StaticJsonDocument<2048> doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error)
  {
    SA_LOG_ERROR("[ServoUART] Erreur JSON servos : %s\n", error.c_str());
    return;
  }

  for (uint8_t servo = 0; servo < 6; ++servo)
  {
    Aig *aig = Settings::canton->getAig(servo);
    if (!aig)
      continue;

    uint8_t exsaAdresse =
        (aig->cantonPdroitIdx() == Settings::canton->SP1_idx()) ? 0 : 1;

    char keyPosDroit[16];
    char keyPosDevie[16];
    char keySpeed[16];

    snprintf(keyPosDroit, sizeof(keyPosDroit), "aig%uposDroit", servo);
    snprintf(keyPosDevie, sizeof(keyPosDevie), "aig%uposDevie", servo);
    snprintf(keySpeed, sizeof(keySpeed), "aig%uspeed", servo);

    // Lecture JSON (source de vérité)
    uint16_t posDroit = doc[keyPosDroit] | aig->posDroit();
    uint16_t posDevie = doc[keyPosDevie] | aig->posDevie();

    // speed = slider 0–10
    uint16_t speedSlider = doc[keySpeed] | 5;

    // Conversion Exploration 2026 → EXSA
    uint16_t speed = 11000 - (speedSlider * 1000);

    envoyerServoConfig(exsaAdresse, servo, posDroit, posDevie, speed);

    SA_LOG_INFO("[ServoUART] F1 → EXSA %u, servo %u (D:%u V:%u S:%u)\n",
                exsaAdresse,
                servo,
                posDroit,
                posDevie,
                speed);
  }

  SA_LOG_INFO("[ServoUART] Configuration servos envoyée\n");
}

/*-------------------------------------------------------------
  Commande F1 : configuration servo
--------------------------------------------------------------*/
void envoyerServoConfig(uint8_t exsaAdresse,
                        uint8_t servoIndex,
                        uint16_t posDroit,
                        uint16_t posDevie,
                        uint16_t speed)
{
  SA_LOG_INFO("[ServoUART] Config : exsa=%u servo=%u droit=%u devie=%u speed=%u\n",
              exsaAdresse, servoIndex, posDroit, posDevie, speed);

  Serial1.write(PROTO_SYNC_BYTE);
  Serial1.write(PROTO_F1_SERVO_CONFIG);
  Serial1.write(exsaAdresse);
  Serial1.write(servoIndex);

  Serial1.write(posDroit >> 8);
  Serial1.write(posDroit & 0xFF);

  Serial1.write(posDevie >> 8);
  Serial1.write(posDevie & 0xFF);

  Serial1.write(speed >> 8);
  Serial1.write(speed & 0xFF);
}

/*-------------------------------------------------------------
  Commande F2 : test servo
--------------------------------------------------------------*/
void envoyerServoTest(uint8_t exsaAdresse, uint8_t servoIndex)
{
  SA_LOG_INFO("[ServoUART] Test : exsa=%u servo=%u\n", exsaAdresse, servoIndex);

  Serial1.write(PROTO_SYNC_BYTE);
  Serial1.write(PROTO_F2_SERVO_TEST);
  Serial1.write(exsaAdresse);
  Serial1.write(servoIndex);
}
