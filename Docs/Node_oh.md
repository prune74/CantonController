/*
 * ============================================================================
 *  Node.h — Version 2026
 *  --------------------------------------------------------------------------
 *  Le Node représente un CANTON complet dans le SA (Satellite d’Aiguillage).
 *
 *  Il contient :
 *    - la topologie SP/SM (voisins amont/aval)
 *    - les aiguilles logiques (pilotées physiquement par EXSA)
 *    - les signaux H et AH
 *    - les capteurs ponctuels
 *    - l’occupation logique (ConsoCourant)
 *    - les paramètres ferroviaires (rôle, vitesse max, sens)
 *
 *  IMPORTANT 2026 :
 *    - Le SA ne pilote plus les servos localement.
 *    - aigRun() envoie une trame RS485 à EXSA (F0/F1/F2).
 *    - Les objets Aig sont purement LOGIQUES.
 *    - EXSA exécute physiquement les mouvements.
 *
 *  Le Node est donc :
 *    → un conteneur de LOGIQUE ferroviaire
 *    → un conteneur de TOPOLOGIE
 *    → un conteneur de CAPTEURS
 *    → un conteneur de SIGNAUX
 *
 *  Il ne contient AUCUNE action physique.
 * ============================================================================
 */

#pragma once

#include <Arduino.h>

#include "Aig.h"       // Aiguilles logiques (sans servo)
#include "Config.h"    // Constantes globales
#include "Loco.h"      // Informations loco (adresse Railcom)
#ifdef RFID
#include "RFID.h"
#endif
#include "Sensor.h"    // Capteurs ponctuels
#include "Signal.h"    // Signaux ferroviaires

class ConsoCourant;    // Déclaration anticipée


/* ============================================================================
 *                           CantonRole
 *  --------------------------------------------------------------------------
 *  Rôle ferroviaire du canton (configurable via interface Web).
 *
 *  Ce rôle influence :
 *    - le type de signaux
 *    - les aspects par défaut
 *    - les règles d’accès
 * ============================================================================
 */
enum CantonRole : uint8_t {
    ROLE_PLEINE_VOIE = 0,   // Aucun signal (voie neutre)
    ROLE_BAL,               // Signalisation BAL
    ROLE_GARE,              // Canton de gare (signal simple)
    ROLE_ENTREE_GARE,       // Entrée de gare (BAL + ralentissement)
    ROLE_SORTIE_GARE,       // Sortie de gare (rappel)
    ROLE_MANOEUVRE,         // Zone de manœuvre (signal MAN)
    ROLE_SERVICE            // Voie de service (simple)
};


/* ============================================================================
 *                           NodePeriph
 *  --------------------------------------------------------------------------
 *  Représente un VOISIN dans la topologie :
 *    - SP1 (amont horaire)
 *    - SM1 (amont anti-horaire)
 *    - SP2 (aval horaire)
 *    - SM2 (aval anti-horaire)
 *
 *  Chaque NodePeriph contient :
 *    - ID du voisin
 *    - busy() : occupation logique du voisin
 *    - acces() : autorisation d’accès
 *    - reserved() : loco réservée dans ce voisin
 *    - masqueAig : aiguilles bloquantes pour cet accès
 *    - aspectRecu[] : aspect SNCF reçu du voisin (H/AH)
 *
 *  Utilisé dans :
 *    - la topologie Discovery
 *    - la supervision canton
 *    - la logique d’itinéraires
 * ============================================================================
 */
class NodePeriph
{
protected:
  uint8_t m_id;          // ID du voisin (0..255)
  bool m_busy;           // Occupation logique du voisin
  uint16_t m_reserved;   // Adresse loco réservée
  bool m_acces;          // Accès autorisé ?
  uint16_t m_locoAddr;   // Adresse loco détectée dans ce voisin
  byte m_masqueAig;      // Aiguilles bloquantes
  byte m_signal;         // Type de signal du voisin
  byte m_typeCible;      // Type de cible (non utilisé en 2026)

public:
  NodePeriph();
  ~NodePeriph();
  static uint8_t comptInst; // Debug : nombre d’instances

  // Aspect SNCF reçu du voisin (H=0 / AH=1)
  uint16_t aspectRecu[2] = {0, 0};

  // --- Getters / Setters ---
  void ID(uint8_t);
  uint8_t ID();

  void busy(bool);
  bool busy();

  void reserved(uint16_t);
  uint16_t reserved();

  void acces(bool);
  bool acces();

  void locoAddr(uint16_t);
  uint16_t locoAddr();

  void masqueAig(byte);
  byte masqueAig();
};


/* ============================================================================
 *                           Node (canton principal)
 *  --------------------------------------------------------------------------
 *  Le Node est le cœur du SA :
 *
 *    - Topologie SP/SM
 *    - Aiguilles logiques (pilotées par EXSA)
 *    - Signaux H/AH
 *    - Capteurs ponctuels
 *    - Occupation logique (ConsoCourant)
 *    - Rôle ferroviaire
 *    - Paramètres (vitesse max, sens)
 *
 *  IMPORTANT :
 *    Node hérite de Aig uniquement pour compatibilité historique.
 *    → Les servos NE SONT PLUS pilotés localement.
 * ============================================================================
 */
class Node : public Aig
{
  friend class Discovery; // Discovery peut modifier la topologie

private:
  // --- Identité ---
  uint16_t m_id;          // ID du canton
  bool m_busy;            // Occupation logique (mise à jour par ConsoCourant)
  uint16_t m_reserved;    // Adresse loco réservée
  byte m_masqueAig;       // Aiguilles bloquantes SP1/SM1

  // --- Topologie automatique ---
  uint8_t m_SP1_idx;      // Index du voisin SP1 dans nodeP[]
  uint8_t m_SM1_idx;      // Index du voisin SM1 dans nodeP[]
  bool m_SP2_acces;       // SP2 accessible ?
  bool m_SP2_busy;        // SP2 occupé ?
  bool m_SM2_acces;       // SM2 accessible ?
  bool m_SM2_busy;        // SM2 occupé ?
  byte m_masqueAigSP2;    // Aiguilles bloquantes SP2
  byte m_masqueAigSM2;    // Aiguilles bloquantes SM2

  // --- Paramètres ferroviaires ---
  uint8_t m_maxSpeed;     // Vitesse max autorisée
  uint8_t m_sensMarche;   // Sens de marche (0 = inconnu)

  // --- Rôle ferroviaire ---
  CantonRole m_role = ROLE_PLEINE_VOIE;

public:
  Node();
  ~Node();

  // --- Voisins, aiguilles, capteurs, signaux ---
  NodePeriph *nodeP[nodePsize];   // Tableau des voisins SP/SM
  Aig *aig[aigSize];              // Aiguilles logiques (sans servo)
  Loco loco;                      // Informations loco (Railcom)
  Sensor sensor[sensorSize];      // Capteurs ponctuels
  Signal *signal[signalSize];     // Signaux H/AH
  ConsoCourant* occupation;       // Capteur courant (UART EXSA)

  uint8_t feuDirection[2] = {0, 0}; // Feux blancs directionnels

  // --- Identité ---
  void ID(uint16_t);
  uint16_t ID();

  // --- Occupation ---
  void busy(bool);
  bool busy();

  void reserved(uint16_t);
  uint16_t reserved();

  void masqueAig(byte);
  byte masqueAig();

  void masqueAigSP2(byte);
  byte masqueAigSP2();

  void masqueAigSM2(byte);
  byte masqueAigSM2();

  // --- Gestion des aiguilles ---
  // aigRun() → envoie une trame RS485 F0 vers EXSA
  // aigGoTo() → obsolète (pilotage EXSA uniquement)
  static void aigGoTo(void *);
  void aigRun(byte);

  // --- Topologie ---
  void SP1_idx(uint8_t);
  uint8_t SP1_idx();

  void SM1_idx(uint8_t);
  uint8_t SM1_idx();

  void SP2_acces(bool);
  bool SP2_acces();

  void SP2_busy(bool);
  bool SP2_busy();

  void SM2_acces(bool);
  bool SM2_acces();

  void SM2_busy(bool);
  bool SM2_busy();

  // --- Paramètres ferroviaires ---
  void maxSpeed(uint8_t);
  uint8_t maxSpeed();

  void sensMarche(uint8_t);
  uint8_t sensMarche();

  // --- Rôle ferroviaire ---
  void role(CantonRole r) { m_role = r; }
  CantonRole role() const { return m_role; }

  // --- Méthodes de configuration ---
  void setRole(CantonRole role);     // setter avec applyRoleDefaults()
  void applyRoleDefaults();          // applique les cibles par défaut


  void debugTopologieEtAiguilles();  // affiche la topologie et les aiguilles

};
