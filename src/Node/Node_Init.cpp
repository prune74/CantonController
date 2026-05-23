/*
 * Node_Init.cpp — Initialisation du canton (Node)
 * ---------------------------------------------------------------------------
 */

#include "Node_Internal.h"
#include "debug_sa.h"   // pour SA_LOG_TRACE / SA_LOG_INFO / SA_LOG_WARN

Node::Node()
    : m_id(UNUSED_ID),
      m_busy(false),
      m_reserved(0),
      m_masqueAig(0x00),
      m_SP1_idx(0),
      m_SM1_idx(0),
      m_SP2_acces(true),
      m_SP2_busy(false),
      m_SM2_acces(true),
      m_SM2_busy(false),
      m_masqueAigSP2(0x00),
      m_masqueAigSM2(0x00),
      m_maxSpeed(128),
      m_sensMarche(SensHoraire),
      m_role(ROLE_PLEINE_VOIE),
      nodeP{nullptr, nullptr, nullptr, nullptr},
      aig{nullptr, nullptr, nullptr, nullptr},
      signal{nullptr, nullptr},
      sensor{},
      loco(nullptr),
      occupation(nullptr)
{
    SA_LOG_TRACE("[Node] Construction du canton (ID=%u)\n", m_id);

    // ----------------------------------------------------------------------
    // Capteurs ponctuels virtuels (EXSA → SA)
    // ----------------------------------------------------------------------
    sensor[IDX_CAPT_ANTIHORAIRE].setup(GPIO_NUM_NC);
    sensor[IDX_CAPT_HORAIRE].setup(GPIO_NUM_NC);

    // ----------------------------------------------------------------------
    // Loco interne
    // ----------------------------------------------------------------------
    loco = new Loco();

    // ----------------------------------------------------------------------
    // Capteur d’occupation (courant) — EXSA UART
    // ----------------------------------------------------------------------
    occupation = new ConsoCourant;
    occupation->setup(this); //occupation->setup(this, CONSO_COURANT_PIN);
    occupation->startReceptionUART();

    SA_LOG_TRACE("[Node] Capteurs virtuels + Loco + ConsoCourant initialisés\n");
}

Node::~Node()
{
    delete occupation;
    delete loco;
    SA_LOG_TRACE("[Node] Destruction du canton (ID=%u)\n", m_id);
}

// ---------------------------------------------------------------------------
// Identité du canton (ID Node)
// ---------------------------------------------------------------------------

void Node::ID(uint16_t id)
{
    m_id = id;
}

uint16_t Node::ID()
{
    return m_id;
}

// ---------------------------------------------------------------------------
// Validation de la topologie (SP1_idx / SM1_idx)
// ---------------------------------------------------------------------------
void Node::validateTopology()
{
    if (m_SP1_idx >= 4)
    {
        SA_LOG_WARN("[Node %u] SP1_idx invalide (%u) → remis à 0\n",
                    m_id, m_SP1_idx);
        m_SP1_idx = 0;
    }

    if (m_SM1_idx >= 4)
    {
        SA_LOG_WARN("[Node %u] SM1_idx invalide (%u) → remis à 0\n",
                    m_id, m_SM1_idx);
        m_SM1_idx = 0;
    }
}

// ---------------------------------------------------------------------------
// Détection du sens de marche initial à partir des capteurs virtuels
// ---------------------------------------------------------------------------
void Node::detectInitialDirection()
{
    bool ah = sensor[IDX_CAPT_ANTIHORAIRE].state();
    bool h  = sensor[IDX_CAPT_HORAIRE].state();

    if (ah && !h)
    {
        m_sensMarche = SensAntiHoraire;
        SA_LOG_INFO("[Node %u] Sens initial détecté : anti‑horaire\n", m_id);
    }
    else if (h && !ah)
    {
        m_sensMarche = SensHoraire;
        SA_LOG_INFO("[Node %u] Sens initial détecté : horaire\n", m_id);
    }
    else
    {
        SA_LOG_TRACE("[Node %u] Sens initial indéterminé\n", m_id);
    }
}

void Node::logInitialState()
{
    SA_LOG_INFO("==============================================\n");
    SA_LOG_INFO("[Node %u] Démarrage du canton\n", m_id);
    SA_LOG_INFO("Rôle ferroviaire : %u\n", m_role);
    SA_LOG_INFO("SP1_idx=%u | SM1_idx=%u\n", m_SP1_idx, m_SM1_idx);
    SA_LOG_INFO("Capteur AH=%d | H=%d\n",
                sensor[IDX_CAPT_ANTIHORAIRE].state(),
                sensor[IDX_CAPT_HORAIRE].state());
    SA_LOG_INFO("Occupation initiale : %d\n", m_busy);
    SA_LOG_INFO("==============================================\n");
}
