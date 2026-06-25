#pragma once
#include <Arduino.h>
#include "Protocol.h" // Protocole commun CC ↔ EXCC

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
#define PIN_CAN_TX GPIO_NUM_4
#define PIN_CAN_RX GPIO_NUM_5
#define CAN_BITRATE 500000UL

/* ============================================================================
 *  CAN — Bus EXCC
 * ============================================================================
 */
#define PIN_EXCC_CS GPIO_NUM_13
#define PIN_EXCC_INT GPIO_NUM_14
#define PIN_EXCC_SCK GPIO_NUM_18  // Obligatoire
#define PIN_EXCC_MOSI GPIO_NUM_23 // Obligatoire
#define PIN_EXCC_MISO GPIO_NUM_19 // Obligatoire

#define QUARTZ_MCP2515 8000000 // MCP2515 quartz 8 MHz
#define CAN_BITRATE_MCP2515 250000UL

/* ============================================================================
 *  DIMENSIONS DES TABLEAUX INTERNES
 * ============================================================================
 */
static constexpr uint8_t cantonPsize = 8; // Cantons périphériques
static constexpr uint8_t aigSize = 6;     // Aiguilles logiques
static constexpr uint8_t sensorSize = 2;  // Capteurs ponctuels
static constexpr uint8_t signalSize = 2;  // Signaux H / AH

/* ============================================================================
 *  MCP23017
 * ============================================================================
 */

#define MCP23017_ADDR 0x20

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