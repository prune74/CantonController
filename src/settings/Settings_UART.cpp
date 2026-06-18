/*
 * Settings_UART.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Gestion de l’UART RS485 utilisé pour communiquer avec le EXCC.
 *
 * Rôle :
 *   - initialiser l’UART matériel (baudrate, format, broches)
 *   - fournir un accès centralisé via Settings::uart()
 *   - assurer une initialisation robuste et loggée
 *
 * Module indépendant :
 *   - pas de JSON
 *   - pas de SPIFFS
 *   - pas de CAN
 *
 * Appelé depuis Settings::setup().
 */

#include "Settings.h"
#include "Config.h"
#include "debug_cc.h"
#include <HardwareSerial.h>

// UART matériel du Canton Controller (port défini dans Config.h)
HardwareSerial Settings::SerialUART(UART_PORT_NUM);

/* ============================================================================
 *  uart() — Accès centralisé à l’UART RS485
 * ==========================================================================*/
HardwareSerial &Settings::uart()
{
    return SerialUART;
}

/* ============================================================================
 *  setupUART() — Initialisation de l’UART RS485
 * ==========================================================================*/
void Settings::setupUART()
{
    SerialUART.begin(
        UART_BAUDRATE,
        SERIAL_8N1,
        UART_RX_CANTON,
        UART_TX_CANTON
    );

    CC_LOG_INFO(
        "[Settings][UART][CC] Initialisé sur UART%d à %lu bauds (RX=%d, TX=%d)\n",
        UART_PORT_NUM,
        UART_BAUDRATE,
        UART_RX_CANTON,
        UART_TX_CANTON
    );
}
