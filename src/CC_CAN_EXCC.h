#pragma once
#include <stdint.h>

#include "Canton.h"
#include "CanMsg.h"

namespace CC_CAN_EXCC
{
    // -----------------------------------------------------------------------
    // Fonction interne : envoi sur le bus EXCC (bus 1)
    // -----------------------------------------------------------------------
    void sendEXCC(uint8_t prio, uint8_t cmde, uint8_t resp,
                  uint16_t id,
                  const uint8_t *data = nullptr,
                  uint8_t len = 0);

    // -----------------------------------------------------------------------
    // Commandes CC → EXCC
    // -----------------------------------------------------------------------
    void sendPing(Canton *canton);
    void sendBoosterPower(Canton *canton, bool on);
    void sendRecalibration(Canton *canton);
    void sendSeuilsBooster(Canton *canton,
                           uint16_t libre,
                           uint16_t occupe);

    // -----------------------------------------------------------------------
    // Aiguilles / mouvement réel (CMD_EXCC_CC_POSITION_AIGUILLE)
    // -----------------------------------------------------------------------
    void sendAiguillesDepuisEtatCourant();
    void sendPositionAiguille(Canton *canton,
                              uint8_t servoIndex,
                              uint8_t direction);

    // -----------------------------------------------------------------------
    // Aspects SNCF (CMD_CC_EXCC_ASPECT_HORAIRE / CMD_CC_EXCC_ASPECT_ANTIHORAIRE)
    // -----------------------------------------------------------------------
    void sendAspectsDepuisEtatCourant();
    void sendAspectHoraire(Canton *canton, uint8_t aspect);
    void sendAspectAntiHoraire(Canton *canton, uint8_t aspect);

    // -----------------------------------------------------------------------
    // Feux directionnels (CMD_CC_EXCC_DIRECTION_HORAIRE / CMD_CC_EXCC_DIRECTION_ANTIHORAIRE)
    // -----------------------------------------------------------------------
    void sendFeuxDepuisEtatCourant();
    void sendFeuDirectionHoraire(Canton *canton, uint8_t code);
    void sendFeuDirectionAntiHoraire(Canton *canton, uint8_t code);

    // -----------------------------------------------------------------------
    // Occupation voisins (CC_EXCC_OCCUPATION_VOISINS)
    // -----------------------------------------------------------------------
    void sendOccVoisinsDepuisEtatCourant();
    void sendOccupationVoisins(Canton *canton, uint8_t valeur);

    // -----------------------------------------------------------------------
    // Servos (CMD_CC_EXCC_SERVO_MOVE / CMD_CC_EXCC_SERVO_CONFIG / CMD_CC_EXCC_SERVO_TEST)
    // -----------------------------------------------------------------------
    void sendConfigurationServosDepuisSettings();
    void sendServoMove(Canton *canton,
                       uint8_t servoIndex,
                       uint8_t direction);

    void sendServoConfig(Canton *canton,
                         uint8_t servoIndex,
                         uint16_t posDroit,
                         uint16_t posDevie,
                         uint16_t speed);

    void sendServoTest(Canton *canton,
                       uint8_t servoIndex);

    // -----------------------------------------------------------------------
    // Configuration signaux (CMD_CC_EXCC_CONFIG_SIGNAUX)
    // -----------------------------------------------------------------------
    void sendConfigurationSignauxDepuisSettings();
    void sendConfigSignaux(Canton *canton,
                           uint8_t type0, uint8_t type1,
                           uint8_t pos0, uint8_t pos1);

    // -----------------------------------------------------------------------
    // Réception EXCC → CC
    // -----------------------------------------------------------------------
    void handleEXCCCommand(uint8_t commande,
                           const CanMsg &frame,
                           Canton *canton,
                           uint16_t idExpediteur);
}
