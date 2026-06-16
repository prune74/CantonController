/*
 * Canton_Init.cpp — Initialisation du canton (Canton)
 * ---------------------------------------------------------------------------
 */

#include "Canton.h"
#include "Config.h"
#include "debug_sa.h"
#include "ConsoCourant.h"

// ---------------------------------------------------------------------------
// Définition du singleton Canton (un seul canton par SA)
// ---------------------------------------------------------------------------
Canton *Canton::s_instance = nullptr;

Canton::Canton()
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
      cantonP{},
      aig{},
      signal{},
      sensor{},
      loco(nullptr),
      occupation(nullptr)
{
    // ----------------------------------------------------------------------
    // 🔥 Initialisation du singleton
    // ----------------------------------------------------------------------
    s_instance = this;

    SA_LOG_TRACE("[Canton] Construction du canton (ID=%u)\n", m_id);

    // ----------------------------------------------------------------------
    // Création des CantonPeriph (voisins)
    // ----------------------------------------------------------------------
    for (uint8_t i = 0; i < cantonPsize; i++)
    {
        cantonP[i] = new CantonPeriph();
        cantonP[i]->ID(UNUSED_ID); // par défaut : absent
    }

    // ----------------------------------------------------------------------
    // Création des aiguilles logiques
    // ----------------------------------------------------------------------
    for (uint8_t i = 0; i < aigSize; i++)
    {
        aig[i] = new Aig();
        // Aig est déjà correctement initialisée dans son constructeur
    }

    // ----------------------------------------------------------------------
    // Création des signaux (AH = 0, H = 1)
    // ----------------------------------------------------------------------
    signal[0] = new Signal(); // AH
    signal[1] = new Signal(); // H

    // ----------------------------------------------------------------------
    // Capteurs ponctuels virtuels (EXSA → SA)
    // ----------------------------------------------------------------------
    // ⚠️ En Exploration 2026, les capteurs sont VIRTUELS (PROTO_03)
    // → Pas de GPIO
    // → Pas de Sensor::setup()
    // → L’état est mis à jour via SA_UartRx

    // ----------------------------------------------------------------------
    // Loco interne
    // ----------------------------------------------------------------------
    loco = new Loco();

    // ----------------------------------------------------------------------
    // Capteur d’occupation (courant) — EXSA UART
    // ----------------------------------------------------------------------
    occupation = new ConsoCourant;
    occupation->setup(this);

    SA_LOG_TRACE("[Canton] CantonPeriph + Aig + Signal + Loco + ConsoCourant initialisés\n");
}

Canton::~Canton()
{
    // ----------------------------------------------------------------------
    // 🔥 Destruction propre (jamais en cours d’exécution FreeRTOS)
    // ----------------------------------------------------------------------
    for (uint8_t i = 0; i < cantonPsize; i++)
        delete cantonP[i];

    for (uint8_t i = 0; i < aigSize; i++)
        delete aig[i];

    delete signal[0];
    delete signal[1];

    delete occupation;
    delete loco;

    SA_LOG_TRACE("[Canton] Destruction du canton (ID=%u)\n", m_id);
}

// ---------------------------------------------------------------------------
// Identité du canton (ID Canton)
// ---------------------------------------------------------------------------

void Canton::ID(uint16_t id)
{
    m_id = id;
}

uint16_t Canton::ID()
{
    return m_id;
}

// ---------------------------------------------------------------------------
// Validation de la topologie (SP1_idx / SM1_idx)
// ---------------------------------------------------------------------------
void Canton::validateTopology()
{
    if (m_SP1_idx >= cantonPsize)
    {
        SA_LOG_WARN("[Canton %u] SP1_idx invalide (%u) → remis à 0\n",
                    m_id, m_SP1_idx);
        m_SP1_idx = 0;
    }

    if (m_SM1_idx >= cantonPsize)
    {
        SA_LOG_WARN("[Canton %u] SM1_idx invalide (%u) → remis à 0\n",
                    m_id, m_SM1_idx);
        m_SM1_idx = 0;
    }
}

// ---------------------------------------------------------------------------
// Détection du sens de marche initial à partir des capteurs virtuels
// ---------------------------------------------------------------------------
void Canton::detectInitialDirection()
{
    bool ah = sensor[IDX_CAPT_ANTIHORAIRE].state();
    bool h = sensor[IDX_CAPT_HORAIRE].state();

    if (ah && !h)
    {
        m_sensMarche = SensAntiHoraire;
        SA_LOG_INFO("[Canton %u] Sens initial détecté : anti‑horaire\n", m_id);
    }
    else if (h && !ah)
    {
        m_sensMarche = SensHoraire;
        SA_LOG_INFO("[Canton %u] Sens initial détecté : horaire\n", m_id);
    }
    else
    {
        SA_LOG_TRACE("[Canton %u] Sens initial indéterminé\n", m_id);
    }
}

void Canton::logInitialState()
{
    SA_LOG_INFO("==============================================\n");
    SA_LOG_INFO("[Canton %u] Démarrage du canton\n", m_id);
    SA_LOG_INFO("Rôle ferroviaire : %u\n", m_role);
    SA_LOG_INFO("SP1_idx=%u | SM1_idx=%u\n", m_SP1_idx, m_SM1_idx);
    SA_LOG_INFO("Capteur AH=%d | H=%d\n",
                sensor[IDX_CAPT_ANTIHORAIRE].state(),
                sensor[IDX_CAPT_HORAIRE].state());
    SA_LOG_INFO("Occupation initiale : %d\n", m_busy);
    SA_LOG_INFO("==============================================\n");
}
