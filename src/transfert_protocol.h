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
    EXPLORATION_REQUEST_CC_ID = 0xC0,   // CC → ERM
    EXPLORATION_MASQUE_AIG    = 0xC1,   // CC ↔ CC

    /* Exploration CC ↔ CC */
    EXPLORATION_ID_VOISIN     = 0xC0,   // CC → CC (même valeur, autre flux)

    /* Exploitation CC ↔ CC */
    EXPLOIT_UPDATE_VOISINS    = 0xE0,
    EXPLOIT_RESERVATION       = 0xE3,
    EXPLOIT_RAILCOM_VOISIN    = 0xE5,
    EXPLOIT_ASPECT_VOISIN     = 0xE7,
    EXPLOIT_AIGUILLAGE        = 0xE9,
    CC_CC_OCCUPATION_VOISINS  = 0xEA,

    /* EXCC → CC */
    CC_EXCC_PING              = 0xD0,
    EXCC_CC_PONG              = 0xD1,
    EXCC_CC_BOOSTER_INFO      = 0xD2,
    EXCC_CC_POSITION_AIGUILLE = 0xD6,
    EXCC_CC_OCCUPATION        = 0xD7,
    EXCC_CC_PONCTUEL_H        = 0xD8,
    EXCC_CC_PONCTUEL_AH       = 0xD9,
    EXCC_CC_RAILCOM_ADRESSE   = 0xDA,
    EXCC_CC_CALIB_BOOSTER_INFO= 0xDB,

    /* CC → EXCC */
    CC_EXCC_SERVO_MOVE        = 0xF0,
    CC_EXCC_SERVO_CONFIG      = 0xF1,
    CC_EXCC_SERVO_TEST        = 0xF2,
    CC_EXCC_RECALIBRER_BOOSTER= 0xF3,
    CC_EXCC_SET_SEUILS        = 0xF4,
    CC_EXCC_BOOSTER_POWER     = 0xF5,
    CC_EXCC_CONFIG_SIGNAUX    = 0xF6,
    CC_EXCC_ASPECT_HORAIRE    = 0xF7,
    CC_EXCC_ASPECT_ANTIHORAIRE= 0xF8,
    CC_EXCC_DIRECTION_HORAIRE = 0xF9,
    CC_EXCC_DIRECTION_ANTIHORAIRE = 0xFA,

    /* Gestion globale ERM ↔ CC */
    WIFI_ON_OFF               = 0xBD,
    EXPLORATION_ON_OFF        = 0xBE,
    SAVE_ALL                  = 0xBF,
    RESTART_ALL               = 0xBC,
    SET_PROFILE               = 0x20,
    CC_OFFLINE                = 0xC3,

    /* SAT */
    SAT_TEST_BUS              = 0xB2,
    SAT_TEST_BUS_REPLY        = 0xB3,
    SAT_REQUEST_ID            = 0xB4,
    SAT_REQUEST_ID_REPLY      = 0xB5,
};

/* ============================================================================
 *  CODES INTERNES EXCC — ENUM CLASS
 * ==========================================================================*/
enum class ExccCode : uint8_t
{
    POS_DROIT        = 0x00,
    POS_DEVIE        = 0x01,
    POS_INDET        = 0x02,
    POS_INCOHERENT   = 0x03,

    PONCT_H_ACTIVE   = 0x10,
    PONCT_H_INACTIVE = 0x11,
    PONCT_AH_ACTIVE  = 0x12,
    PONCT_AH_INACTIVE= 0x13,

    OCC_ACTIVE       = 0x30,
    OCC_LIBRE        = 0x31,

    ETAT_OK          = 0x00,
    ETAT_BLOQUE      = 0x01,
    ETAT_ERREUR      = 0x02,
};

/* ============================================================================
 *  CAN 11 bits — constexpr
 * ==========================================================================*/
constexpr uint16_t CAN_ID_HEARTBEAT      = 0x200;
constexpr uint16_t CAN_ID_EMERGENCY_STOP = 0x201;
constexpr uint16_t CAN_ID_CLEAR_STOP     = 0x202;

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

Dans chaque fichier, remplacer 0xE3 par CanCmd::EXPLOIT_RESERVATION

Remplacer 0xC0 par CanCmd::EXPLORATION_ID_VOISIN ou CanCmd::EXPLORATION_REQUEST_CC_ID selon le flux

Compiler après chaque petit groupe de remplacements pour vérifier

03
Adapter les comparaisons
Important
Les enum class sont typés, donc il faut caster pour comparer.

Exemple : if (cmd == static_cast<uint8_t>(CanCmd::EXPLOIT_RESERVATION))

Ou mieux : stocker CanCmd cmd = static_cast<CanCmd>(msg.cmde());

04
Mettre à jour les handlers
Les switch doivent maintenant utiliser les enums.

CC_CAN.cpp → handleExploitCommand / handleExplorationCommand

switch (cmd) devient switch (static_cast<CanCmd>(cmd))

Les cases deviennent : case CanCmd::EXPLOIT_RESERVATION:

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