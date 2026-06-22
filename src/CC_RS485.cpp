/*
 * CC_RS485.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Couche bas-niveau RS485 pour la communication CC ↔ EXCC.
 *
 * Rôle :
 *   - initialiser l’UART RS485
 *   - gérer la direction du transceiver (DE/RE)
 *   - envoyer des octets ou des trames
 *   - fournir un readByte() non bloquant
 *
 * Ce module ne contient aucune logique métier :
 *   → il transporte uniquement les données brutes.
 */

#include "CC_RS485.h"
#include "Config.h"
#include "Settings.h"
#include "debug_cc.h"

// ---------------------------------------------------------------------------
// Référence vers l’UART configuré dans Settings
// ---------------------------------------------------------------------------
static HardwareSerial &uart = Settings::uart();

// ---------------------------------------------------------------------------
// Initialisation RS485
// ---------------------------------------------------------------------------
void CC_RS485::begin() // 🟢
{
    pinMode(RS485_DE_RE, OUTPUT);
    digitalWrite(RS485_DE_RE, LOW); // réception par défaut

    uart.begin(UART_BAUDRATE, SERIAL_8N1, UART_RX_CANTON, UART_TX_CANTON);

    CC_LOG_INFO("[CC_RS485][CC] RS485 initialisé (baud=%lu)\n", UART_BAUDRATE);
}

// ---------------------------------------------------------------------------
// Passage en émission
// ---------------------------------------------------------------------------
void CC_RS485::setTx() // 🟢
{
    digitalWrite(RS485_DE_RE, HIGH);
    delayMicroseconds(50);
}

// ---------------------------------------------------------------------------
// Passage en réception
// ---------------------------------------------------------------------------
void CC_RS485::setRx() // 🟢
{
    delayMicroseconds(50);
    digitalWrite(RS485_DE_RE, LOW);
}

// ---------------------------------------------------------------------------
// Envoi d’un octet
// ---------------------------------------------------------------------------
void CC_RS485::sendByte(uint8_t b) // 🔴 
{
    setTx();
    uart.write(b);
    uart.flush();
    setRx();
}

// ---------------------------------------------------------------------------
// Envoi d’une trame complète
// ---------------------------------------------------------------------------
void CC_RS485::sendFrame(const uint8_t *data, size_t len) // 🟢
{
    setTx();
    uart.write(data, len);
    uart.flush();
    setRx();
}

// ---------------------------------------------------------------------------
// Lecture non bloquante d’un octet
// ---------------------------------------------------------------------------
int CC_RS485::readByte() // 🟢
{
    if (uart.available())
        return uart.read();

    return -1;
}
