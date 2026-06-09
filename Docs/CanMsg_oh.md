/*
  CanMsg.h

  Structure des identifiants CAN :
  https://www.locoduino.org/IMG/png/satautonomes_messageriecan_v1.png

  ----------------------------------------------------------------------
  NOTE IMPORTANTE (architecture 2026) :

  Le SA ne pilote plus les servos localement.
  Les commandes d’aiguillage (CAN 0xE9) sont désormais routées vers EXSA
  via RS485, avec adressage (0 = EXSA_H, 1 = EXSA_AH).

  Le bus RS485 est commun aux deux EXSA universels.
  Le routage et l’envoi des trames RS485 sont gérés dans CanMsg.cpp.
  ----------------------------------------------------------------------
*/

#pragma once

#include <ACAN_ESP32.h>
#include "Config.h"
#include "Exploration.h"
#include "Settings.h"
#include "Canton.h"

class CanMsg
{
public:
  CanMsg() = delete;
  static void setup(Canton *);
  static void testMemory(void *);
  static void canReceiveMsg(void *);
  static void sendMsg(CANMessage &);
  static void sendMsg(byte, byte, byte, uint16_t);
  static void sendMsg(byte, byte, byte, uint16_t, byte);
  static void sendMsg(byte, byte, byte, uint16_t, byte, byte);
  static void sendMsg(byte, byte, byte, uint16_t, byte, byte, byte);
  static void sendMsg(byte, byte, byte, uint16_t, byte, byte, byte, byte);
  static void sendMsg(byte, byte, byte, uint16_t, byte, byte, byte, byte, byte);
  static void sendMsg(byte, byte, byte, uint16_t, byte, byte, byte, byte, byte, byte);
  static void sendMsg(byte, byte, byte, uint16_t, byte, byte, byte, byte, byte, byte, byte);
  static void sendMsg(byte, byte, byte, uint16_t, byte, byte, byte, byte, byte, byte, byte, byte);
};
