#pragma once
#include <stdint.h>

/* ============================================================================
 *  ENUM ASPECTS SNCF
 * ==========================================================================*/
enum class ExccAspect : uint8_t
{
    CARRE = 0,
    CARRE_VIOLET,
    SEMAPHORE,
    AVERTISSEMENT,
    RALENTISSEMENT_30,
    RALENTISSEMENT_60,
    RAPPEL_30,
    RAPPEL_60,
    VOIE_LIBRE,
    MANOEUVRE,
    MASQUE,
};

/* ============================================================================
 *  COMMANDES CAN 29 bits — ENUM CLASS
 * ==========================================================================*/
enum class CanCmd : uint16_t
{
    /* Exploration CC ↔ ERM */
    CMD_EXPLORATION_CC_DEMANDE_ID = 0xC0, // CC → ERM

    /* Exploration CC ↔ CC */
    CMD_EXPLORATION_ID_VOISIN = 0xC0,         // CC → CC (même valeur, autre flux)
    CMD_EXPLORATION_UPDATE_MASQUE_AIG = 0xC1, // CC ↔ CC

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
    CC_EXCC_PING = 0xD0,

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
constexpr uint16_t CAN_ID_HEARTBEAT = 0x200;
constexpr uint16_t CAN_ID_EMERGENCY_STOP = 0x201;
constexpr uint16_t CAN_ID_CLEAR_STOP = 0x202;

/*
🟩 Méthode officielle pour migrer ton protocole vers enum class
01
Créer les nouveaux `enum class` et `constexpr`
Point de départ
On commence par ajouter les versions modernes dans Protocol.h, sans supprimer les anciens #define.

Ajouter enum class CanCmd avec toutes les commandes

Ajouter enum class ExccCode pour les codes internes

Ajouter constexpr pour les IDs 11 bits

Garder les #define TEMPORAIREMENT pour éviter de casser le code existant

02
Remplacer les valeurs brutes dans le code
Progressif
On remplace progressivement les 0xE3, 0xC0, etc. par les versions typées.

Dans chaque fichier, remplacer 0xE3 par CanCmd::EXPLOITATION_RESERVATION

Remplacer 0xC0 par CanCmd::EXPLORATION_ID_VOISIN ou CanCmd::EXPLORATION_REQUEST_CC_ID selon le flux

Compiler après chaque petit groupe de remplacements pour vérifier

03
Adapter les comparaisons
Important
Les enum class sont typés, donc il faut caster pour comparer.

Exemple : if (cmd == static_cast<uint8_t>(CanCmd::EXPLOITATION_RESERVATION))

Ou mieux : stocker CanCmd cmd = static_cast<CanCmd>(msg.cmde());

04
Mettre à jour les handlers
Les switch doivent maintenant utiliser les enums.

CC_CAN.cpp → handleExploitCommand / handleExplorationCommand

switch (cmd) devient switch (static_cast<CanCmd>(cmd))

Les cases deviennent : case CanCmd::EXPLOITATION_RESERVATION:

05
Supprimer les anciens #define
Quand plus aucun fichier n’utilise les macros, tu peux les retirer.

Supprimer les #define CMD_...

Garder uniquement les enum class et constexpr

Recompiler pour vérifier qu’il ne reste aucune référence

06
Nettoyer et valider
Finalisation
On vérifie que tout est cohérent et lisible.

Vérifier que tous les fichiers utilisent les enums

Vérifier que les logs affichent bien les valeurs

Vérifier que les handlers reçoivent les bonnes commandes
*/