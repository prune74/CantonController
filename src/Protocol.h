#pragma once
#include <stdint.h>

/*
 * ============================================================================
 *  Protocol.h — Gestion Canton 2026
 * ============================================================================
 *  Fichier partagé entre :
 *    • ExplorationMaster (ERM)
 *    • Canton Controller (CC)
 *    • Extension Canton Controller (EXCC)
 *
 *  Objectifs :
 *    → cohérence totale du protocole Discovery 2026
 *    → séparation claire des flux :
 *         - CC ↔ ERM (exploration / supervision)
 *         - CC ↔ CC (exploitation)
 *         - CC ↔ EXCC (actionneurs)
 *    → éviter toute collision d’opcodes
 * ============================================================================
 */

/* ============================================================================
 *  🟦 ENUM DES ASPECTS SNCF — 1 octet
 * ============================================================================
 */

enum ExccAspect : uint8_t
{
    ASPECT_CARRE = 0,
    ASPECT_CARRE_VIOLET,
    ASPECT_SEMAPHORE,
    ASPECT_AVERTISSEMENT,
    ASPECT_RALENTISSEMENT_30,
    ASPECT_RALENTISSEMENT_60,
    ASPECT_RAPPEL_30,
    ASPECT_RAPPEL_60,
    ASPECT_VOIE_LIBRE,
    ASPECT_MANOEUVRE,
    ASPECT_MASQUE,
};

/* ============================================================================
 *  🟦 CC ↔ ERM — Mode Exploration (0xC0–0xCF)
 * ============================================================================
 */

#define CMD_EXPLORATION_REQUEST_CC_ID 0xC0
#define CMD_EXPLORATION_MASQUE_AIG 0xC1

/* ============================================================================
 *  🟦 CC ↔ CC — Exploitation (0xE0–0xEF)
 * ============================================================================
 */

#define CMD_EXPLOIT_UPDATE_VOISINS 0xE0
#define CMD_EXPLOIT_RESERVATION 0xE3
#define CMD_EXPLOIT_RAILCOM_VOISIN 0xE5
#define CMD_EXPLOIT_ASPECT_VOISIN 0xE7
#define CMD_EXPLOIT_AIGUILLAGE 0xE9

/* Occupation SP/SM */
#define CMD_CC_CC_OCCUPATION_VOISINS 0xEA

/* Un CC annonce son ID aux voisins */
#define CMD_EXPLORATION_ID_VOISIN 0xC0

/* ============================================================================
 *  🟦 CC → EXCC — Actionneurs physiques (0xF0–0xFF)
 * ============================================================================
 */

#define CMD_CC_EXCC_SERVO_MOVE 0xF0
#define CMD_CC_EXCC_SERVO_CONFIG 0xF1
#define CMD_CC_EXCC_SERVO_TEST 0xF2

#define CMD_CC_EXCC_RECALIBRER_BOOSTER 0xF3
#define CMD_CC_EXCC_SET_SEUILS 0xF4
#define CMD_CC_EXCC_BOOSTER_POWER 0xF5

#define CMD_CC_EXCC_CONFIG_SIGNAUX 0xF6
#define CMD_CC_EXCC_ASPECT_HORAIRE 0xF7
#define CMD_CC_EXCC_ASPECT_ANTIHORAIRE 0xF8
#define CMD_CC_EXCC_DIRECTION_HORAIRE 0xF9
#define CMD_CC_EXCC_DIRECTION_ANTIHORAIRE 0xFA

/* ============================================================================
 *  🟦 EXCC → CC — Retour d’état (0xD0–0xDF)
 * ============================================================================
 */

#define CMD_CC_EXCC_PING 0xD0
#define CMD_EXCC_CC_PONG 0xD1
#define CMD_EXCC_CC_BOOSTER_INFO 0xD2
#define CMD_EXCC_CC_POSITION_AIGUILLE 0xD6
#define CMD_EXCC_CC_OCCUPATION 0xD7
#define CMD_EXCC_CC_PONCTUEL_H 0xD8
#define CMD_EXCC_CC_PONCTUEL_AH 0xD9
#define CMD_EXCC_CC_RAILCOM_ADRESSE 0xDA
#define CMD_EXCC_CC_CALIB_BOOSTER_INFO 0xDB

/* ============================================================================
 *  🟦 Codes internes EXCC (payload)
 * ============================================================================
 */

#define EXCC_CODE_POS_DROIT 0x00
#define EXCC_CODE_POS_DEVIE 0x01
#define EXCC_CODE_POS_INDET 0x02
#define EXCC_CODE_POS_INCOHERENT 0x03

#define EXCC_CODE_PONCT_H_ACTIVE 0x10
#define EXCC_CODE_PONCT_H_INACTIVE 0x11
#define EXCC_CODE_PONCT_AH_ACTIVE 0x12
#define EXCC_CODE_PONCT_AH_INACTIVE 0x13

#define EXCC_CODE_OCC_ACTIVE 0x30
#define EXCC_CODE_OCC_LIBRE 0x31

#define EXCC_CODE_ETAT_OK 0x00
#define EXCC_CODE_ETAT_BLOQUE 0x01
#define EXCC_CODE_ETAT_ERREUR 0x02

/* ============================================================================
 *  🟦 ERM ↔ CC — Gestion globale (0x20–0xBF)
 * ============================================================================
 */

#define CMD_ERM_CC_WIFI_ON_OFF 0xBD
#define CMD_ERM_CC_EXPLORATION_ON_OFF 0xBE
#define CMD_ERM_CC_SAVE_ALL 0xBF
#define CMD_ERM_CC_RESTART_ALL 0xBC
#define CMD_ERM_CC_SET_PROFILE 0x20
#define CMD_ERM_CC_OFFLINE 0xC3

/* ============================================================================
 *  🟦 CC ↔ ERM — Gestion globale (0x20–0xBF)
 * ============================================================================
 */
// Test de la présence de la carte ERM
#define CMD_CC_ERM_TEST_BUS 0xB2

// Reponse à la demande de la présence de la carte ERM
#define CMD_ERM_CC_TEST_BUS_REPLY 0xB3

// Demande d’ID si nécessaire
#define CMD_CC_ERM_REQUEST_ID 0xB4

// Reponse à demande d'identifiant
#define CMD_ERM_CC_REQUEST_ID 0xB5

/* ============================================================================
 *  🟦 CAN 11 bits — Messages globaux
 * ============================================================================
 */

#define EXPLORATION_CAN_ID_HEARTBEAT 0x200
#define EXPLORATION_CAN_ID_EMERGENCY_STOP 0x201
#define EXPLORATION_CAN_ID_CLEAR_STOP 0x202
