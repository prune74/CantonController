#pragma once
#include <stdint.h>

/*
 * ============================================================================
 *  Exploration_Protocol.h — Gestion Canton 2026
 * ============================================================================
 *  Fichier unique partagé entre :
 *    • ExplorationMaster (Master)
 *    • Canton Controller (CC)
 *    • Extension Canton Controller (EXCC)
 *
 *  Contient :
 *    • Aspects SNCF (enum ExccAspect)
 *    • Protocole CC ↔ EXCC (UART 0xAA / OPCODE)
 *    • Protocole Master ↔ CC (CAN 29 bits)
 *    • Messages globaux Exploration (CAN 11 bits)
 *
 *  Objectifs :
 *    → garantir une cohérence totale du protocole 2026
 *    → éviter les divergences entre firmwares
 *    → faciliter la maintenance et l’évolution
 * ============================================================================
 */

/* ============================================================================
 *  🟦 ENUM DES ASPECTS SNCF — 1 octet
 * ============================================================================
 */

enum ExccAspect : uint8_t
{
    ASPECT_CARRE = 0,         // 🔴 Arrêt absolu
    ASPECT_CARRE_VIOLET,      // 🟣 Carré violet - Arrêt absolu
    ASPECT_SEMAPHORE,         // 🔴 Arrêt
    ASPECT_AVERTISSEMENT,     // 🟡 Ralentir
    ASPECT_RALENTISSEMENT_30, // 🟡⚠️ 30 km/h
    ASPECT_RALENTISSEMENT_60, // 🟡⚠️ 60 km/h
    ASPECT_RAPPEL_30,         // 🟡🔁 Rappel 30
    ASPECT_RAPPEL_60,         // 🟡🔁 Rappel 60
    ASPECT_VOIE_LIBRE,        // 🟢 Voie libre
    ASPECT_MANOEUVRE,         // 🔵 Manoeuvre
    ASPECT_MASQUE,            // ⚫ Masqué
};

/* ============================================================================
 *  🟧 PROTOCOLE CC ↔ EXCC — UART (0xAA + OPCODE)
 * ============================================================================
 */

#define PROTO_SYNC_BYTE 0xAA

/* --- Supervision --- */
#define PROTO_PING 0x32
#define PROTO_PONG 0x33

/* --- Topologie / Configuration --- */
#define PROTO_E4_TOPOLOGIE_CAN 0xE4
#define PROTO_E5_CONFIG_SIGNAUX 0xE5

/* --- Aspects SNCF --- */
#define PROTO_E6_ASPECT_HORAIRE 0xE6
#define PROTO_E7_ASPECT_ANTIHORAIRE 0xE7

/* --- Feux directionnels --- */
#define PROTO_E8_DIRECTION_HORAIRE 0xE8
#define PROTO_E9_DIRECTION_ANTIHORAIRE 0xE9

/* --- Occupation voisins --- */
#define PROTO_EA_OCCUPATION_VOISINS 0xEA

/* --- Servos (pilotés physiquement par EXCC) --- */
#define PROTO_F0_SERVO_MOVE 0xF0
#define PROTO_F1_SERVO_CONFIG 0xF1
#define PROTO_F2_SERVO_TEST 0xF2

/* --- Booster (CC → EXCC) --- */
#define PROTO_F3_RECALIBRER_BOOSTER 0xF3
#define PROTO_F4_SET_SEUILS 0xF4
#define PROTO_F5_BOOSTER_POWER 0xF5

/* ============================================================================
 *  🟩 EXCC → CC : Informations remontées
 * ============================================================================
 */

#define PROTO_03_H_PONCTUEL 0x03
#define PROTO_03_AH_PONCTUEL 0x04
#define PROTO_04_OCCUPATION 0x05
#define PROTO_05_COMPTEUR_ESSIEUX 0x06
#define PROTO_06_POSITION_AIGUILLE 0x07
#define PROTO_07_BOOSTER 0x08
#define PROTO_08_RAILCOM_ADRESSE 0x09
#define PROTO_09_CALIB_BOOSTER 0x0A

/* --- Codes associés --- */
#define PROTO_PONCT_H_ACTIVE 0x10
#define PROTO_PONCT_H_INACTIVE 0x11
#define PROTO_PONCT_AH_ACTIVE 0x12
#define PROTO_PONCT_AH_INACTIVE 0x13

/* --- Occupation --- */
#define PROTO_OCC_ACTIVE 0x30
#define PROTO_OCC_LIBRE 0x31

#define PROTO_POS_DROIT 0x00
#define PROTO_POS_DEVIE 0x01
#define PROTO_POS_INDET 0x02
#define PROTO_POS_INCOHERENT 0x03

#define PROTO_ETAT_OK 0x00
#define PROTO_ETAT_BLOQUE 0x01
#define PROTO_ETAT_ERREUR 0x02

/* ============================================================================
 *  🟦 PROTOCOLE MASTER ↔ CC — Bus CAN 29 bits
 * ============================================================================
 */

/* --- Master → CC --- */
#define CMD_WIFI_ON_OFF 0xBD
#define CMD_EXPLORATION_ON_OFF 0xBE
#define CMD_SAVE_ALL 0xBF
#define CMD_RESTART_ALL 0xBC
#define CMD_SET_PROFILE 0x20

/* --- CC → Master --- */
#define CMD_SAT_TEST_BUS 0xB2
#define CMD_SAT_TEST_BUS_REPLY 0xB3
#define CMD_SAT_REQUEST_ID 0xB4
#define CMD_SAT_REQUEST_ID_REPLY 0xB5

/* ============================================================================
 *  🟥 EXPLORATION CAN 11 bits — Messages globaux Master ↔ CC
 * ============================================================================
 */

/* Heartbeat CC → Master */
#define EXPLORATION_CAN_ID_HEARTBEAT 0x200

/* STOP global Master → CC (puis CC → EXCC) */
#define EXPLORATION_CAN_ID_EMERGENCY_STOP 0x201

/* CLEAR STOP global Master → CC */
#define EXPLORATION_CAN_ID_CLEAR_STOP 0x202
