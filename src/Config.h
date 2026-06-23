#pragma once
#include <Arduino.h>
#include "Exploration_Protocol.h" // Protocole commun CC ↔ EXCC

/*
 * ============================================================================
 *  Config.h — Gestion Canton 2026
 * ============================================================================
 *  Paramètres globaux du Canton Controller (CC).
 *
 *  Contient :
 *    - métadonnées projet
 *    - options générales
 *    - index topologie SP/SM
 *    - constantes CAN / UART / RS485
 *    - tailles des tableaux internes
 *    - mapping MCP23017 (Exploration)
 *
 *  Objectif :
 *    → centraliser toutes les constantes du CC
 *    → garantir une cohérence totale entre modules
 * ============================================================================
 */

/* ============================================================================
 *  Niveaux de logs
 * ============================================================================
 */
#define CC_DEBUG 0 // 0 = aucun log, 1 = INFO/WARN/ERROR, 2 = + TRACE

/* ============================================================================
 *  MÉTADONNÉES PROJET
 * ============================================================================
 */
#define PROJECT "Gestion Canton 2026"
#define VERSION "v 0.1"
#define AUTHOR "Bruno"

/* ============================================================================
 *  OPTIONS GÉNÉRALES
 * ============================================================================
 */
#define SAUV_BY_MAIN // Sauvegarde via carte Main
#define CHIP_INFO    // Affiche infos ESP32 au boot
#define RAILCOM      // Active RailCom
// #define TEST_MEMORY_TASK

/* ============================================================================
 *  MODE STANDALONE (sans Master)
 * ============================================================================
 */
#define CC_STANDALONE_MODE 1

/* ============================================================================
 *  INDEX DES SATELLITES (topologie SP/SM)
 * ============================================================================
 *  pXX = voisins sens horaire (SP)
 *  mXX = voisins sens anti‑horaire (SM)
 * ============================================================================
 */
enum : uint8_t
{
    p00,
    p01,
    p10,
    p11, // SP
    m00,
    m01,
    m10,
    m11 // SM
};

/* ============================================================================
 *  IDENTIFIANTS CAN
 * ============================================================================
 */
#define CENTRALE_DCC_ID 253
#define UNUSED_ID 255
static constexpr uint8_t NODE_UNUSED_ID = UNUSED_ID;

/* ============================================================================
 *  BROCHES ESP32-WROVER
 * ============================================================================
 */
#define UNUSED_PIN 255

/* ============================================================================
 *  CAN — Bus Exploration 2026
 * ============================================================================
 */
#define CAN_RX GPIO_NUM_4
#define CAN_TX GPIO_NUM_5
#define CAN_BITRATE 250000UL

/* ============================================================================
 *  DIMENSIONS DES TABLEAUX INTERNES
 * ============================================================================
 */
static constexpr uint8_t cantonPsize = 8; // Cantons périphériques
static constexpr uint8_t aigSize = 6;     // Aiguilles logiques
static constexpr uint8_t sensorSize = 2;  // Capteurs ponctuels
static constexpr uint8_t signalSize = 2;  // Signaux H / AH

/* ============================================================================
 *  RAILCOM — Détection adresse loco
 * ============================================================================
 */
#define NB_ADDRESS_TO_COMPARE 100
#define RAILCOM_RX GPIO_NUM_32
#define RAILCOM_TX GPIO_NUM_33

/* ============================================================================
 *  UART → EXCC (signaux + servos)
 * ============================================================================
 */
#define UART_TX_CANTON GPIO_NUM_25
#define UART_RX_CANTON GPIO_NUM_26
#define UART_BAUDRATE 9600UL
#define UART_PORT_NUM 1

/* ============================================================================
 *  RS485 — Direction du transceiver
 * ============================================================================
 */
#define RS485_DE_RE GPIO_NUM_27

/* ============================================================================
 *  MCP23017
 * ============================================================================
 */

// Boutons Exploration
#define MCP_PIN_BTN_SAT_MOINS 0 // GPA0
#define MCP_PIN_BTN_SAT_PLUS 1  // GPA1
#define MCP_PIN_INTER_DEV_2 2   // GPA2
#define MCP_PIN_INTER_DEV_1 3   // GPA3

// Mode Manoeuvre
#define MCP_PIN_BTN_MANOEUVRE 4 // GPA4

// LED Exploration
#define MCP_PIN_LED_EXPLORATION 5 // GPA5

// LED Manoeuvre
#define MCP_PIN_LED_MANOEUVRE 6 // GPA6

// LED Topologie
#define MCP_PIN_LED_TOPOLOGIE 7 // GPA7