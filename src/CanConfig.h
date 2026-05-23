/*
  CanConfig.h - Configuration du CAN de l'ESP32
*/

#pragma once

#include <Arduino.h>
#include <ACAN_ESP32.h>
#include "Config.h"

class CanConfig
{
private:
public:
  CanConfig() = delete;
  static void setup();
};
