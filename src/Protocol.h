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
 *  ENUM ASPECTS SNCF
 * ==========================================================================*/
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
 *  COMMANDES CAN 29 bits — ENUM CLASS
 * ==========================================================================*/
enum class CanCmd : uint16_t
{
    /* Exploration CC ↔ ERM */
    CMD_EXPLORATION_CC_DEMANDE_ID = 0xC0,

    /* Exploration CC ↔ CC */
    CMD_EXPLORATION_ID_VOISIN = 0xC0,
    CMD_EXPLORATION_UPDATE_MASQUE_AIG = 0xC1,

    /* Exploitation CC ↔ CC */
    CMD_EXPLOITATION_UPDATE_VOISINS = 0xE0,
    CMD_EXPLOITATION_RESERVATION_LOCO = 0xE3,
    CMD_EXPLOITATION_RAILCOM_VOISIN = 0xE5,
    CMD_EXPLOITATION_ASPECT_VOISIN = 0xE7,
    CMD_EXPLOITATION_AIGUILLAGE = 0xE9,

    /* EXCC → CC */
    EXCC_CC_PONG = 0xD1,
    EXCC_CC_BOOSTER_INFO = 0xD2,
    EXCC_CC_POSITION_AIGUILLE = 0xD6,
    EXCC_CC_OCCUPATION = 0xD7,
    EXCC_CC_PONCTUEL_H = 0xD8,
    EXCC_CC_PONCTUEL_AH = 0xD9,
    EXCC_CC_RAILCOM_ADRESSE = 0xDA,
    EXCC_CC_CALIB_BOOSTER_INFO = 0xDB,

    /* CC → EXCC */
    CC_EXCC_SERVO_MOVE = 0xF0,
    CC_EXCC_SERVO_CONFIG = 0xF1,
    CC_EXCC_SERVO_TEST = 0xF2,
    CC_EXCC_RECALIBRER_BOOSTER = 0xF3,
    CC_EXCC_SET_SEUILS = 0xF4,
    CC_EXCC_BOOSTER_POWER = 0xF5,
    CC_EXCC_CONFIG_SIGNAUX = 0xF6,
    CC_EXCC_ASPECT_HORAIRE = 0xF7,
    CC_EXCC_ASPECT_ANTIHORAIRE = 0xF8,
    CC_EXCC_DIRECTION_HORAIRE = 0xF9,
    CC_EXCC_DIRECTION_ANTIHORAIRE = 0xFA,
    CC_EXCC_OCCUPATION_VOISINS = 0xFB,
    CC_EXCC_PING = 0xFC,

    /* Gestion globale ERM ↔ CC */
    CMD_ERM_CC_WIFI_ON_OFF = 0xBD,
    CMD_ERM_CC_EXPLORATION_ON_OFF = 0xBE,
    CMD_ERM_CC_SAVE_ALL = 0xBF,
    CMD_ERM_CC_RESTART_ALL = 0xBC,
    CMD_ERM_CC_SET_PROFILE = 0x20,
    CMD_ERM_CC_OFFLINE = 0xC3,

    /* Demande d’ID si nécessaire */
    CMD_CC_ERM_TEST_BUS = 0xB2,

    /* Reponse à la demande de la présence de la carte ERM */
    CMD_ERM_CC_TEST_BUS_REPLY = 0xB3,

    /* Demande d’ID si nécessaire */
    CMD_CC_ERM_REQUEST_ID = 0xB4,

    /* Reponse à demande d'identifiant */
    CMD_ERM_CC_REQUEST_ID = 0xB5,
};

/* ============================================================================
 *  CODES INTERNES EXCC — ENUM CLASS
 * ==========================================================================*/
enum class ExccCode : uint8_t
{
    POS_DROIT = 0x00,
    POS_DEVIE = 0x01,
    POS_INDET = 0x02,
    POS_INCOHERENT = 0x03,

    PONCT_H_ACTIVE = 0x10,
    PONCT_H_INACTIVE = 0x11,
    PONCT_AH_ACTIVE = 0x12,
    PONCT_AH_INACTIVE = 0x13,

    OCC_ACTIVE = 0x30,
    OCC_LIBRE = 0x31,

    ETAT_OK = 0x00,
    ETAT_BLOQUE = 0x01,
    ETAT_ERREUR = 0x02,
};

/* ============================================================================
 *  CAN 11 bits — constexpr
 * ==========================================================================*/
constexpr uint16_t CAN11_ID_HEARTBEAT = 0x200;
constexpr uint16_t CAN11_ID_EMERGENCY_STOP = 0x201;
constexpr uint16_t CAN11_ID_CLEAR_STOP = 0x202;