#include "CC_CAN_EXCC.h"
#include "Booster.h"
#include "EXCC_Link.h"
#include "SupervisionAiguilles.h"
#include "Occupation.h"
#include "CapteurPonctuel.h"
#include "Railcom.h"
#include "debug_cc.h"
#include "CanBus.h"
#include "CanID.h"
#include "CanMsg.h"
#include "Canton.h"

// ---------------------------------------------------------------------------
// Fonction interne : envoi sur le bus EXCC (bus 1)
// ---------------------------------------------------------------------------
void CC_CAN_EXCC::sendEXCC(uint8_t prio, uint8_t cmde, uint8_t resp,
                           uint16_t id, const uint8_t *data, uint8_t len)
{
    uint32_t id29 = CanID::make29(prio, cmde, resp, id);
    CanMsg msg(id29, {});

    for (uint8_t i = 0; i < len; i++)
        msg.data[i] = data[i];

    msg.dlc = len;

    CanBus::bus(1).send(msg); // EXCC = bus 1
}

// ---------------------------------------------------------------------------
// Envoie des commandes CC → EXCC
// ---------------------------------------------------------------------------
void CC_CAN_EXCC::sendPing(Canton *canton)
{
    sendEXCC(1, static_cast<uint8_t>(CanCmd::CC_EXCC_PING), 0, canton->ID());
}

void CC_CAN_EXCC::sendBoosterPower(Canton *canton, bool on)
{
    uint8_t payload[1] = {uint8_t(on ? 1 : 0)};
    sendEXCC(1, static_cast<uint8_t>(CanCmd::CC_EXCC_BOOSTER_POWER),
             0, canton->ID(), payload, 1);
}

void CC_CAN_EXCC::sendRecalibration(Canton *canton)
{
    sendEXCC(1, static_cast<uint8_t>(CanCmd::CC_EXCC_RECALIBRER_BOOSTER),
             0, canton->ID());
}

void CC_CAN_EXCC::sendSeuilsBooster(Canton *canton,
                                    uint16_t libre,
                                    uint16_t occupe)
{
    uint8_t payload[4] = {
        uint8_t(libre & 0xFF),
        uint8_t(libre >> 8),
        uint8_t(occupe & 0xFF),
        uint8_t(occupe >> 8)};

    sendEXCC(1, static_cast<uint8_t>(CanCmd::CC_EXCC_SET_SEUILS),
             0, canton->ID(), payload, 4);
}

// ---------------------------------------------------------------------------
// Aiguilles / mouvement réel (CMD_EXCC_CC_POSITION_AIGUILLE)
// ---------------------------------------------------------------------------
void CC_CAN_EXCC::sendPositionAiguille(Canton *canton,
                                       uint8_t servoIndex,
                                       uint8_t direction)
{
    uint8_t payload[2] = {servoIndex, direction};
    sendEXCC(1, static_cast<uint8_t>(CanCmd::EXCC_CC_POSITION_AIGUILLE),
             0, canton->ID(), payload, 2);
}

// ---------------------------------------------------------------------------
// Aspects SNCF
// ---------------------------------------------------------------------------
void CC_CAN_EXCC::sendAspectHoraire(Canton *canton, uint8_t aspect)
{
    uint8_t payload[1] = {aspect};
    sendEXCC(1, static_cast<uint8_t>(CanCmd::CC_EXCC_ASPECT_HORAIRE),
             0, canton->ID(), payload, 1);
}

void CC_CAN_EXCC::sendAspectAntiHoraire(Canton *canton, uint8_t aspect)
{
    uint8_t payload[1] = {aspect};
    sendEXCC(1, static_cast<uint8_t>(CanCmd::CC_EXCC_ASPECT_ANTIHORAIRE),
             0, canton->ID(), payload, 1);
}

// ---------------------------------------------------------------------------
// Feux directionnels
// ---------------------------------------------------------------------------
void CC_CAN_EXCC::sendFeuDirectionHoraire(Canton *canton, uint8_t code)
{
    uint8_t payload[1] = {code};
    sendEXCC(1, static_cast<uint8_t>(CanCmd::CC_EXCC_DIRECTION_HORAIRE),
             0, canton->ID(), payload, 1);
}

void CC_CAN_EXCC::sendFeuDirectionAntiHoraire(Canton *canton, uint8_t code)
{
    uint8_t payload[1] = {code};
    sendEXCC(1, static_cast<uint8_t>(CanCmd::CC_EXCC_DIRECTION_ANTIHORAIRE),
             0, canton->ID(), payload, 1);
}

// ---------------------------------------------------------------------------
// Occupation voisins
// ---------------------------------------------------------------------------
void CC_CAN_EXCC::sendOccupationVoisins(Canton *canton, uint8_t valeur)
{
    uint8_t payload[1] = {valeur};
    sendEXCC(1, static_cast<uint8_t>(CanCmd::CC_EXCC_OCCUPATION_VOISINS),
             0, canton->ID(), payload, 1);
}

// ---------------------------------------------------------------------------
// Servos
// ---------------------------------------------------------------------------
void CC_CAN_EXCC::sendServoMove(Canton *canton,
                                uint8_t servoIndex,
                                uint8_t direction)
{
    uint8_t payload[2] = {servoIndex, direction};
    sendEXCC(1, static_cast<uint8_t>(CanCmd::CC_EXCC_SERVO_MOVE),
             0, canton->ID(), payload, 2);
}

void CC_CAN_EXCC::sendServoConfig(Canton *canton,
                                  uint8_t servoIndex,
                                  uint16_t posDroit,
                                  uint16_t posDevie,
                                  uint16_t speed)
{
    uint8_t payload[7] = {
        servoIndex,
        uint8_t(posDroit >> 8), uint8_t(posDroit & 0xFF),
        uint8_t(posDevie >> 8), uint8_t(posDevie & 0xFF),
        uint8_t(speed >> 8), uint8_t(speed & 0xFF)};

    sendEXCC(1, static_cast<uint8_t>(CanCmd::CC_EXCC_SERVO_CONFIG),
             0, canton->ID(), payload, 7);
}

void CC_CAN_EXCC::sendServoTest(Canton *canton,
                                uint8_t servoIndex)
{
    uint8_t payload[1] = {servoIndex};
    sendEXCC(1, static_cast<uint8_t>(CanCmd::CC_EXCC_SERVO_TEST),
             0, canton->ID(), payload, 1);
}

// ---------------------------------------------------------------------------
// Configuration signaux
// ---------------------------------------------------------------------------
void CC_CAN_EXCC::sendConfigSignaux(Canton *canton,
                                    uint8_t type0, uint8_t type1,
                                    uint8_t pos0, uint8_t pos1)
{
    uint8_t payload[4] = {type0, type1, pos0, pos1};
    sendEXCC(1, static_cast<uint8_t>(CanCmd::CC_EXCC_CONFIG_SIGNAUX),
             0, canton->ID(), payload, 4);
}

// ---------------------------------------------------------------------------
// Réception des commandes EXCC → CC
// ---------------------------------------------------------------------------
void CC_CAN_EXCC::handleEXCCCommand(uint8_t commande,
                                    const CanMsg &frame,
                                    Canton *canton,
                                    uint16_t idExpediteur)
{
    const uint8_t *d = frame.data;

    // Conversion vers enum class
    CanCmd cmd = static_cast<CanCmd>(commande);

    switch (cmd)
    {
    case CanCmd::EXCC_CC_PONG:
        EXCC_Link::onPong();
        break;

    case CanCmd::EXCC_CC_BOOSTER_INFO:
        if (frame.dlc >= 3)
            Booster::onBooster(d[0], d[1], d[2]);
        break;

    case CanCmd::EXCC_CC_POSITION_AIGUILLE:
        if (frame.dlc >= 3)
            SupervisionAiguilles::onPosition(d[0], d[1], d[2]);
        break;

    case CanCmd::EXCC_CC_OCCUPATION:
        if (frame.dlc >= 1)
            Occupation::onOccupation(d[0]);
        break;

    case CanCmd::EXCC_CC_PONCTUEL_H:
        if (frame.dlc >= 1)
            CapteurPonctuel::onPonctuelH(d[0]);
        break;

    case CanCmd::EXCC_CC_PONCTUEL_AH:
        if (frame.dlc >= 1)
            CapteurPonctuel::onPonctuelAH(d[0]);
        break;

    case CanCmd::EXCC_CC_RAILCOM_ADRESSE:
        if (frame.dlc >= 2)
            Railcom::onRailcom(d[0], d[1]);
        break;

    case CanCmd::EXCC_CC_CALIB_BOOSTER_INFO:
        if (frame.dlc >= 4)
            Booster::onCalib(d[0], d[1], d[2], d[3]);
        break;

    default:
        CC_LOG_WARN("[CC_CAN_EXCC][CC] Cmd EXCC inconnue : 0x%02X\n", commande);
        break;
    }
}
