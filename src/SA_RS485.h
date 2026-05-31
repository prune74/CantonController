#pragma once
#include <Arduino.h>

namespace SA_RS485
{
    void begin();
    void setTx();
    void setRx();
    void sendByte(uint8_t b);
    void sendFrame(const uint8_t* data, size_t len);
    int readByte();
}
