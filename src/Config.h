/*
 * ============================================================================
 *  Config.h — Version 2026 (Option A)
 *  --------------------------------------------------------------------------
 *  Configuration centrale du Satellite Autonome (SA)
 *
 *  Ce fichier regroupe TOUTES les constantes matérielles et logicielles :
 *
 *    - Mapping GPIO ESP32-WROVER
 *    - Paramètres CAN (bitrate, broches)
 *    - Paramètres UART vers EXSA (signaux + servos)
 *    - Paramètres aiguilles (logiques uniquement)
 *    - Paramètres signaux (H / AH)
 *    - Paramètres Discovery (apprentissage topologie)
 *
 *  Ce fichier est inclus dans presque tous les modules du SA.
 *  Il doit rester simple, clair, et strictement déclaratif.
 *
 * ============================================================================
 */

#pragma once
#include <Arduino.h>
#include "Discovery_Protocol.h" // 🔥 Protocole commun SA ↔ EXSA (E4..EA, F0..F2)

// ============================================================
//  Niveaux de logs
// ============================================================
// SA_DEBUG = 0 → tout désactivé
// SA_DEBUG = 1 → INFO/WARN/ERROR
// SA_DEBUG = 2 → + TRACE
// ============================================================
#define SA_DEBUG 2

/* ============================================================================
 *  MÉTADONNÉES PROJET
 *  --------------------------------------------------------------------------
 *  Informations affichées au démarrage du SA.
 *  Utilisées uniquement pour le debug et l’interface Web.
 * ============================================================================
 */
#define PROJECT "Satellites autonomes (client)"
#define VERSION "v 0.15.1"
#define AUTHOR "Christophe Bobille — modifié par Bruno"

/* ============================================================================
 *  OPTIONS GÉNÉRALES
 *  --------------------------------------------------------------------------
 *  SAUV_BY_MAIN :
 *      La sauvegarde settings.json est déclenchée par le Main (commande CMD_SAVE_ALL)
 *
 *  CHIP_INFO :
 *      Affiche les infos ESP32 au démarrage (RAM, flash, révision)
 *
 *  TEST_MEMORY_TASK :
 *      Active une tâche FreeRTOS qui surveille la stack de canReceiveMsg()
 * ============================================================================
 */
#define SAUV_BY_MAIN
#define CHIP_INFO
#define RAILCOM
// #define TEST_MEMORY_TASK

/* ============================================================================
 *  INDEX DES SATELLITES (topologie SP/SM)
 *  --------------------------------------------------------------------------
 *  Ces index sont utilisés dans :
 *      - node->nodeP[]
 *      - Discovery
 *      - GestionReseau
 *
 *  Ils représentent les positions SP/SM dans la matrice topologique.
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
 *  --------------------------------------------------------------------------
 *  CENTRALE_DCC_ID :
 *      ID réservé à la centrale DCC (253)
 *
 *  UNUSED_ID :
 *      ID par défaut d’un SA non configuré
 * ============================================================================
 */
#define CENTRALE_DCC_ID 253
#define UNUSED_ID 255

/* ============================================================================
 *  BROCHES ESP32-WROVER
 *  --------------------------------------------------------------------------
 *  UNUSED_PIN :
 *      Valeur spéciale pour indiquer qu’une broche n’est pas utilisée.
 * ============================================================================
 */
#define UNUSED_PIN 255

/* ============================================================================
 *  CAN — Bus Discovery 2026
 *  --------------------------------------------------------------------------
 *  CAN_RX / CAN_TX :
 *      Broches du transceiver CAN (SN65HVD230 ou équivalent)
 *
 *  CAN_BITRATE :
 *      Débit du bus CAN → 250 kb/s (standard Discovery)
 * ============================================================================
 */
#define CAN_RX GPIO_NUM_4
#define CAN_TX GPIO_NUM_5
#define CAN_BITRATE 250000UL

/* ============================================================================
 *  DIMENSIONS DES TABLEAUX NODE
 *  --------------------------------------------------------------------------
 *  nodePsize :
 *      Nombre maximum de voisins SP/SM
 *
 *  aigSize :
 *      Nombre maximum d’aiguilles logiques
 *
 *  sensorSize :
 *      Nombre de capteurs ponctuels (H / AH)
 *
 *  signalSize :
 *      Nombre de signaux (H / AH)
 * ============================================================================
 */
static constexpr uint8_t nodePsize = 8;
static constexpr uint8_t aigSize = 6;
static constexpr uint8_t sensorSize = 2;
static constexpr uint8_t signalSize = 2;

/* ============================================================================
 *  RAILCOM — Détection adresse loco
 *  --------------------------------------------------------------------------
 *  NB_ADDRESS_TO_COMPARE :
 *      Nombre d’adresses Railcom mémorisées pour filtrage
 *
 *  RAILCOM_RX / RAILCOM_TX :
 *      Broches UART Railcom (vers module Railcom ESP32)
 * ============================================================================
 */
#define NB_ADDRESS_TO_COMPARE 100
#define RAILCOM_RX GPIO_NUM_32
#define RAILCOM_TX GPIO_NUM_33

/* ============================================================================
 *  UART → EXSA (signaux + servos)
 *  --------------------------------------------------------------------------
 *  UART_TX_SATELLITE / UART_RX_SATELLITE :
 *      UART matériel utilisé pour communiquer avec EXSA
 *
 *  UART_BAUDRATE :
 *      9600 bauds (standard EXSA)
 *
 *  UART_SYNC_BYTE :
 *      Octet de synchronisation du protocole commun SA ↔ EXSA
 * ============================================================================
 */
#define UART_TX_SATELLITE GPIO_NUM_25
#define UART_RX_SATELLITE GPIO_NUM_26
#define UART_BAUDRATE 9600UL
#define UART_PORT_NUM 1

#define UART_SYNC_BYTE PROTO_SYNC_BYTE

/* ============================================================================
 *  RS485 — Direction du transceiver (DE/RE)
 * ============================================================================
 */
#define RS485_DE_RE GPIO_NUM_27

/* ============================================================================
 *  ALIAS LOCAUX → PROTOCOLE COMMUN SA ↔ EXSA
 *  --------------------------------------------------------------------------
 *  Ces alias permettent au SA d’utiliser des noms internes cohérents,
 *  tout en mappant vers les trames officielles du protocole EXSA.
 *
 *  Exemple :
 *      UART_TRAME_ASPECT_HORAIRE → PROTO_E6_ASPECT_HORAIRE
 *
 *  Cela évite de modifier tout le code si le protocole évolue.
 * ============================================================================
 */

// Topologie & signaux
#define UART_TRAME_TOPOLOGIE_CAN PROTO_E4_TOPOLOGIE_CAN
#define UART_TRAME_CONFIG_SIGNAUX PROTO_E5_CONFIG_SIGNAUX
#define UART_TRAME_ASPECT_HORAIRE PROTO_E6_ASPECT_HORAIRE
#define UART_TRAME_ASPECT_ANTIHORAIRE PROTO_E7_ASPECT_ANTIHORAIRE
#define UART_TRAME_DIRECTION_HORAIRE PROTO_E8_DIRECTION_HORAIRE
#define UART_TRAME_DIRECTION_ANTIHORAIRE PROTO_E9_DIRECTION_ANTIHORAIRE
#define UART_TRAME_TYPE_OCCUPATION_VOISINS PROTO_EA_OCCUPATION_VOISINS

// Commandes servos
#define UART_TRAME_SERVO_MOVE PROTO_F0_SERVO_MOVE
#define UART_TRAME_SERVO_CONFIG PROTO_F1_SERVO_CONFIG
#define UART_TRAME_SERVO_TEST PROTO_F2_SERVO_TEST

/* ============================================================================
 *  DÉCOUVERTE (Discovery)
 *  --------------------------------------------------------------------------
 *  Broches utilisées pour :
 *      - boutons SAT+ / SAT−
 *      - interrupteurs DEV1 / DEV2
 *      - LED Discovery
 *
 *  Utilisées uniquement en mode apprentissage topologique.
 * ============================================================================
 */
#define INTER_DEV_1 GPIO_NUM_34
#define INTER_DEV_2 GPIO_NUM_39
#define BTN_SAT_PLUS GPIO_NUM_36
#define BTN_SAT_MOINS GPIO_NUM_35
#define LED_PIN_DISCOV GPIO_NUM_16