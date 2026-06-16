#include "SA_RS485.h"
#include "Config.h"
#include "Settings.h"

static HardwareSerial& uart = Settings::uart();

void SA_RS485::begin()
{
    pinMode(RS485_DE_RE, OUTPUT);
    digitalWrite(RS485_DE_RE, LOW);

    uart.begin(UART_BAUDRATE, SERIAL_8N1, UART_RX_SATELLITE, UART_TX_SATELLITE);
}

void SA_RS485::setTx()
{
    digitalWrite(RS485_DE_RE, HIGH);
    delayMicroseconds(50);
}

void SA_RS485::setRx()
{
    delayMicroseconds(50);
    digitalWrite(RS485_DE_RE, LOW);
}

void SA_RS485::sendByte(uint8_t b)
{
    setTx();
    uart.write(b);
    uart.flush();
    setRx();
}

void SA_RS485::sendFrame(const uint8_t* data, size_t len)
{
    setTx();
    uart.write(data, len);
    uart.flush();
    setRx();
}

int SA_RS485::readByte()
{
    if (uart.available())
        return uart.read();
    return -1;
}
