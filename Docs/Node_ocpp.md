/*
 * ============================================================================
 *  Node.cpp — Version 2026
 *  --------------------------------------------------------------------------
 *  Le Node représente un CANTON dans le SA (Satellite d’Aiguillage).
 *
 *  Il contient :
 *    - la topologie SP/SM (voisins amont/aval)
 *    - les aiguilles locales (mais pilotées par EXSA)
 *    - les signaux (H et AH)
 *    - l’occupation logique (fusion capteur courant + essieux)
 *    - les capteurs ponctuels (H et AH)
 *    - les paramètres ferroviaires (rôle, vitesse max, sens)
 *
 *  IMPORTANT 2026 :
 *    - Le SA ne pilote plus les servos localement.
 *    - Toute commande d’aiguille est envoyée à EXSA via RS485.
 *    - L’occupation est gérée par ConsoCourant (UART EXSA).
 *
 * ============================================================================
 */

#include "Node.h"
#include "ConsoCourant.h"
#include "SatTopologieUART.h"   // pour envoyerServoMove() vers EXSA


/* ============================================================================
 *                           NodePeriph
 *  --------------------------------------------------------------------------
 *  Représente un "voisin" dans la topologie :
 *    - SP1 / SP2 / SM1 / SM2
 *    - Chaque NodePeriph correspond à un canton adjacent.
 *
 *  Il stocke :
 *    - ID du voisin
 *    - busy() : occupation logique du voisin
 *    - acces() : autorisation d’accès (itinéraires)
 *    - reserved() : loco réservée dans ce voisin
 *    - masqueAig : aiguilles bloquantes pour cet accès
 *
 * ============================================================================
 */

uint8_t NodePeriph::comptInst = 0;   // compteur d’instances (debug)

NodePeriph::NodePeriph()
    : m_id(UNUSED_ID),
      m_busy(false),
      m_reserved(0),
      m_acces(true),
      m_locoAddr(0),
      m_masqueAig(0x00),
      m_signal(0)
{
  ++comptInst;
}

NodePeriph::~NodePeriph()
{
  --comptInst;
}

// --- Getters / Setters simples ---
void NodePeriph::ID(uint8_t id) { m_id = id; }
uint8_t NodePeriph::ID() { return m_id; }

void NodePeriph::busy(bool busy) { m_busy = busy; }
bool NodePeriph::busy() { return m_busy; }

void NodePeriph::reserved(uint16_t locoAddr) { m_reserved = locoAddr; }
uint16_t NodePeriph::reserved() { return m_reserved; }

void NodePeriph::acces(bool acces) { m_acces = acces; }
bool NodePeriph::acces() { return m_acces; }

void NodePeriph::locoAddr(uint16_t addr) { m_locoAddr = addr; }
uint16_t NodePeriph::locoAddr() { return m_locoAddr; }

void NodePeriph::masqueAig(byte masqueAig) { m_masqueAig = masqueAig; }
byte NodePeriph::masqueAig() { return m_masqueAig; }



/* ============================================================================
 *                                 Node
 *  --------------------------------------------------------------------------
 *  Le Node est le cœur du SA :
 *
 *    - Il représente un canton complet.
 *    - Il contient les voisins SP/SM.
 *    - Il contient les aiguilles locales.
 *    - Il contient les signaux H et AH.
 *    - Il contient les capteurs ponctuels.
 *    - Il contient l’occupation logique (via ConsoCourant).
 *
 *  Le Node ne pilote plus les servos :
 *    → il envoie des commandes à EXSA via RS485.
 *
 * ============================================================================
 */

Node::Node()
    : m_id(UNUSED_ID),
      m_busy(false),          // Occupation logique (mise à jour par ConsoCourant)
      m_reserved(0),          // Loco réservée dans ce canton
      m_masqueAig(0x00),      // Aiguilles bloquantes SP1/SM1
      m_SP1_idx(0),           // Index du voisin SP1 dans nodeP[]
      m_SM1_idx(0),           // Index du voisin SM1 dans nodeP[]
      m_SP2_acces(true),      // SP2 accessible ?
      m_SP2_busy(false),      // SP2 occupé ?
      m_SM2_acces(true),      // SM2 accessible ?
      m_SM2_busy(false),      // SM2 occupé ?
      m_masqueAigSP2(0x00),   // Aiguilles bloquantes SP2
      m_masqueAigSM2(0x00),   // Aiguilles bloquantes SM2
      m_maxSpeed(128),        // Vitesse max autorisée
      m_sensMarche(0),        // Sens de marche (0 = inconnu)
      m_role(ROLE_PLEINE_VOIE),
      occupation(nullptr)
{
  // --- Initialisation des tableaux internes ---
  for (byte i = 0; i < nodePsize; i++)
    nodeP[i] = nullptr;

  for (byte i = 0; i < aigSize; i++)
    aig[i] = nullptr;

  for (byte i = 0; i < signalSize; i++)
    signal[i] = nullptr;

  // --- Capteurs ponctuels (H et AH) ---
  sensor[0].setup(CAPT_PONCT_ANTIHOR_PIN, CAPT_PONCT_TEMPO, INPUT_PULLUP);
  sensor[1].setup(CAPT_PONCT_HORAIRE_PIN, CAPT_PONCT_TEMPO, INPUT_PULLUP);

  // --- Capteur d’occupation (courant) ---
  // ConsoCourant lit l’UART EXSA et met à jour busy()
  occupation = new ConsoCourant;
  occupation->setup(this, CONSO_COURANT_PIN);
  occupation->startReceptionUART();
}

Node::~Node()
{
  delete occupation;
}


// --- Getters / Setters simples ---
void Node::ID(uint16_t id) { m_id = id; }
uint16_t Node::ID() { return m_id; }

void Node::busy(bool busy) { m_busy = busy; }
bool Node::busy() { return m_busy; }

void Node::reserved(uint16_t add_loco) { m_reserved = add_loco; }
uint16_t Node::reserved() { return m_reserved; }

void Node::masqueAig(byte masqueAig) { m_masqueAig = masqueAig; }
byte Node::masqueAig() { return m_masqueAig; }

void Node::masqueAigSP2(byte v) { m_masqueAigSP2 = v; }
byte Node::masqueAigSP2() { return m_masqueAigSP2; }

void Node::masqueAigSM2(byte v) { m_masqueAigSM2 = v; }
byte Node::masqueAigSM2() { return m_masqueAigSM2; }

void Node::SP1_idx(uint8_t idx) { m_SP1_idx = idx; }
uint8_t Node::SP1_idx() { return m_SP1_idx; }

void Node::SM1_idx(uint8_t idx) { m_SM1_idx = idx; }
uint8_t Node::SM1_idx() { return m_SM1_idx; }

void Node::SP2_acces(bool v) { m_SP2_acces = v; }
bool Node::SP2_acces() { return m_SP2_acces; }

void Node::SP2_busy(bool v) { m_SP2_busy = v; }
bool Node::SP2_busy() { return m_SP2_busy; }

void Node::SM2_acces(bool v) { m_SM2_acces = v; }
bool Node::SM2_acces() { return m_SM2_acces; }

void Node::SM2_busy(bool v) { m_SM2_busy = v; }
bool Node::SM2_busy() { return m_SM2_busy; }

void Node::maxSpeed(uint8_t v) { m_maxSpeed = v; }
uint8_t Node::maxSpeed() { return m_maxSpeed; }

void Node::sensMarche(uint8_t v) { m_sensMarche = v; }
uint8_t Node::sensMarche() { return m_sensMarche; }



/* ============================================================================
 *                Gestion des aiguilles (pilotage EXSA)
 *  --------------------------------------------------------------------------
 *  Avant 2026 :
 *      Le SA pilotait les servos localement.
 *
 *  Depuis 2026 :
 *      Le SA envoie une commande RS485 à EXSA :
 *          → EXSA pilote physiquement les servos.
 *
 *  Pourquoi ?
 *      - Centralisation du pilotage
 *      - Réduction du câblage
 *      - Robustesse industrielle
 * ============================================================================
 */

void Node::aigRun(byte idx)
{
    /**************************************************************************
     * Cette fonction NE pilote PLUS les servos localement.
     *
     * Elle envoie une trame RS485 :
     *      [AA][F0][adresse EXSA][index servo]
     *
     * EXSA reçoit la trame, vérifie si elle lui est destinée,
     * puis pilote le servo correspondant.
     **************************************************************************/

    if (aig[idx] == nullptr)
        return;

    // Déterminer quel EXSA doit recevoir la commande
    uint8_t exsaAdresse = 0;

    // Si l’aiguille pointe vers SP1 → EXSA horaire
    if (aig[idx]->nodePdroitIdx() == SP1_idx())
        exsaAdresse = 0;
    else
        exsaAdresse = 1;

    // Envoi de la commande F0 (servoMove)
    envoyerServoMove(exsaAdresse, idx);
}



/* ============================================================================
 *                Tâche de déplacement des aiguilles (désactivée)
 * ============================================================================
 */

void Node::aigGoTo(void *p)
{
    // Ancien système → supprimé
    vTaskDelete(NULL);
}



/* ============================================================================
 *                Gestion du rôle ferroviaire du canton
 *  --------------------------------------------------------------------------
 *  Le rôle détermine :
 *    - le type de signaux
 *    - les aspects par défaut
 *    - les règles d’accès
 *
 *  Exemples :
 *    ROLE_BAL         → signaux BAL
 *    ROLE_ENTREE_GARE → signaux d’entrée
 *    ROLE_MANOEUVRE   → signaux de manœuvre
 * ============================================================================
 */

void Node::setRole(CantonRole role)
{
  m_role = role;

  SA_LOG("[Node %d] setRole() → rôle ferroviaire = %d\n", m_id, m_role);

  applyRoleDefaults();
}

void Node::applyRoleDefaults()
{
  if (signal[0] == nullptr || signal[1] == nullptr)
  {

    SA_LOG("[Node %d] applyRoleDefaults() ignoré : signaux non initialisés\n", m_id);

    return;
  }

  uint8_t h = signal[0]->type();
  uint8_t ah = signal[1]->type();


  SA_LOG("[Node %d] applyRoleDefaults() type=%d | H=%d AH=%d\n",
               m_id, m_role, h, ah);


  switch (m_role)
  {
  case ROLE_BAL:
    if (h == SIG_SIMPLE) signal[0]->type(SIG_BAL);
    if (ah == SIG_SIMPLE) signal[1]->type(SIG_BAL);
    break;

  case ROLE_GARE:
    // Signaux simples
    break;

  case ROLE_ENTREE_GARE:
    if (h == SIG_SIMPLE) signal[0]->type(SIG_ENTREE);
    if (ah == SIG_SIMPLE) signal[1]->type(SIG_ENTREE);
    break;

  case ROLE_SORTIE_GARE:
    if (h == SIG_SIMPLE) signal[0]->type(SIG_SORTIE);
    if (ah == SIG_SIMPLE) signal[1]->type(SIG_SORTIE);
    break;

  case ROLE_MANOEUVRE:
    if (h == SIG_SIMPLE) signal[0]->type(SIG_MANOEUVRE);
    if (ah == SIG_SIMPLE) signal[1]->type(SIG_MANOEUVRE);
    break;

  case ROLE_SERVICE:
  case ROLE_PLEINE_VOIE:
  default:
    // Signaux simples
    break;
  }
}



/* ============================================================================
 *                Debug : affichage complet du canton
 * ============================================================================
 */


void Node::debugTopologieEtAiguilles()
{
    SA_LOG("==============================================");
    SA_LOG("[Node %d] Diagnostic topologie & aiguilles\n", m_id);
    SA_LOG("==============================================");

    // --- SP1 / SM1 ---
    SA_LOG("SP1_idx = %d\n", SP1_idx());
    SA_LOG("SM1_idx = %d\n", SM1_idx());

    // --- SP2 / SM2 ---
    SA_LOG("SP2: acces=%d busy=%d masqueAig=0x%02X\n",
                 SP2_acces(), SP2_busy(), masqueAigSP2());
    SA_LOG("SM2: acces=%d busy=%d masqueAig=0x%02X\n",
                 SM2_acces(), SM2_busy(), masqueAigSM2());

    SA_LOG("----------------------------------------------");
    SA_LOG("Aiguilles :");

    for (uint8_t i = 0; i < aigSize; i++)
    {
        Aig* a = aig[i];
        if (!a)
            continue;

        SA_LOG(" - Aig[%d] : droit=%d devie=%d | estDroit=%d\n",
                     i,
                     a->nodePdroitIdx(),
                     a->nodePdevieIdx(),
                     a->estDroit());

        // Déduction automatique du côté H / AH
        if (a->nodePdroitIdx() == SP1_idx() || a->nodePdevieIdx() == SP1_idx())
            SA_LOG("     → Cette aiguille est côté HORAIRE (SP)");
        else if (a->nodePdroitIdx() == SM1_idx() || a->nodePdevieIdx() == SM1_idx())
            SA_LOG("     → Cette aiguille est côté ANTI-HORAIRE (SM)");
        else
            SA_LOG("     → ⚠️ Aiguille non reliée à SP1 ni SM1 !");
    }

    SA_LOG("----------------------------------------------");
    SA_LOG("Voisins (nodeP[]) :");

    for (uint8_t i = 0; i < nodePsize; i++)
    {
        NodePeriph* p = nodeP[i];
        if (!p)
            continue;

        SA_LOG(" - nodeP[%d] : ID=%d busy=%d acces=%d reserved=%d\n",
                     i,
                     p->ID(),
                     p->busy(),
                     p->acces(),
                     p->reserved());
    }

    SA_LOG("==============================================");
}

