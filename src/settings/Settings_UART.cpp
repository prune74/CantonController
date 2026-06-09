/*
 * Settings_UART.cpp — Gestion UART RS485 (SA → EXSA)
 * ---------------------------------------------------------------------------
 * Rôle :
 *   - Initialiser l’UART matériel utilisé pour communiquer avec les EXSA
 *   - Fournir un accès centralisé via Settings::uart()
 *   - Assurer une initialisation robuste et loggée
 *
 * Ce module est totalement indépendant :
 *   - pas de JSON
 *   - pas de SPIFFS
 *   - pas de CAN
 *
 * Il est appelé depuis Settings::setup().
 */

#include "Settings.h"
#include "Config.h"
#include "debug_sa.h"
#include <HardwareSerial.h>

// UART matériel partagé entre EXSA_H et EXSA_AH
HardwareSerial Settings::SerialUART(1);

/*
 * uart()
 * ---------------------------------------------------------------------------
 * Retourne une référence vers l’UART RS485 utilisé pour la communication
 * SA → EXSA.
 */
HardwareSerial& Settings::uart()
{
    return SerialUART;
}

/*
 * setupUART()
 * ---------------------------------------------------------------------------
 * Initialise l’UART matériel :
 *   - baudrate défini dans Config.h
 *   - format 8N1
 *   - broches RX/TX définies dans Config.h
 *
 * Cette fonction est appelée depuis Settings::setup().
 */
void Settings::setupUART()
{
    SerialUART.begin(
        UART_BAUDRATE,
        SERIAL_8N1,
        UART_RX_SATELLITE,
        UART_TX_SATELLITE
    );

    Serial.printf(
        "[Settings][UART] Initialisé sur UART%d à %lu bauds (RX=%d, TX=%d)\n",
        UART_PORT_NUM,
        UART_BAUDRATE,
        UART_RX_SATELLITE,
        UART_TX_SATELLITE
    );
}
