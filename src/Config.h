#pragma once
#include <Arduino.h>
#include "Exploration_Protocol.h" // Protocole commun SA ↔ EXSA

// ============================================================
//  Niveaux de logs
// ============================================================
#define SA_DEBUG 2

/*
 * ============================================================================
 *  CAN — Handshake Exploration_Master_Board
 * ============================================================================
 */
#define SA_STANDALONE_MODE 1

/* ============================================================================
 *  MÉTADONNÉES PROJET
 * ============================================================================
 */
#define PROJECT "Satellites autonomes (client)"
#define VERSION "v 0.15.1"
#define AUTHOR "Christophe Bobille — modifié par Bruno"

/* ============================================================================
 *  OPTIONS GÉNÉRALES
 * ============================================================================
 */
#define SAUV_BY_MAIN
#define CHIP_INFO
#define RAILCOM
// #define TEST_MEMORY_TASK

/* ============================================================================
 *  INDEX DES SATELLITES (topologie SP/SM)
 * ============================================================================
 */
enum : uint8_t
{
  p00,
  p01,
  p10,
  p11, // Voisins sens horaire (SP)
  m00,
  m01,
  m10,
  m11 // Voisins sens anti-horaire (SM)
};

/* ============================================================================
 *  IDENTIFIANTS CAN
 * ============================================================================
 */
#define CENTRALE_DCC_ID 253
#define UNUSED_ID 255

// ID spécial pour indiquer qu’aucun voisin n’est configuré
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
 *  DIMENSIONS DES TABLEAUX NODE
 * ============================================================================
 */
static constexpr uint8_t cantonPsize = 8; // p00..p11 + m00..m11
static constexpr uint8_t aigSize = 6;     // 3 aiguilles H + 3 aiguilles AH
static constexpr uint8_t sensorSize = 2;  // H / AH
static constexpr uint8_t signalSize = 2;  // H / AH

// Index des capteurs ponctuels
static constexpr uint8_t IDX_CAPT_ANTIHORAIRE = 0;
static constexpr uint8_t IDX_CAPT_HORAIRE = 1;

/* ============================================================================
 *  RAILCOM — Détection adresse loco
 * ============================================================================
 */
#define NB_ADDRESS_TO_COMPARE 100
#define RAILCOM_RX GPIO_NUM_32
#define RAILCOM_TX GPIO_NUM_33

/* ============================================================================
 *  UART → EXSA (signaux + servos)
 * ============================================================================
 */
#define UART_TX_SATELLITE GPIO_NUM_25
#define UART_RX_SATELLITE GPIO_NUM_26
#define UART_BAUDRATE 9600UL
#define UART_PORT_NUM 1
#define UART_SYNC_BYTE PROTO_SYNC_BYTE

/* ============================================================================
 *  RS485 — Direction du transceiver
 * ============================================================================
 */
#define RS485_DE_RE GPIO_NUM_27

/* ============================================================================
 *  ALIAS LOCAUX → PROTOCOLE COMMUN SA ↔ EXSA
 * ============================================================================
 */
#define UART_TRAME_TOPOLOGIE_CAN PROTO_E4_TOPOLOGIE_CAN
#define UART_TRAME_CONFIG_SIGNAUX PROTO_E5_CONFIG_SIGNAUX
#define UART_TRAME_ASPECT_HORAIRE PROTO_E6_ASPECT_HORAIRE
#define UART_TRAME_ASPECT_ANTIHORAIRE PROTO_E7_ASPECT_ANTIHORAIRE
#define UART_TRAME_DIRECTION_HORAIRE PROTO_E8_DIRECTION_HORAIRE
#define UART_TRAME_DIRECTION_ANTIHORAIRE PROTO_E9_DIRECTION_ANTIHORAIRE
#define UART_TRAME_TYPE_OCCUPATION_VOISINS PROTO_EA_OCCUPATION_VOISINS

#define UART_TRAME_SERVO_MOVE PROTO_F0_SERVO_MOVE
#define UART_TRAME_SERVO_CONFIG PROTO_F1_SERVO_CONFIG
#define UART_TRAME_SERVO_TEST PROTO_F2_SERVO_TEST

/* ============================================================================
 *  DÉCOUVERTE (Exploration)
 * ============================================================================
 */
#define INTER_DEV_1 GPIO_NUM_34
#define INTER_DEV_2 GPIO_NUM_39
#define BTN_SAT_PLUS GPIO_NUM_36
#define BTN_SAT_MOINS GPIO_NUM_35
#define LED_PIN_DISCOV GPIO_NUM_16
