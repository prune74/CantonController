/*
 * Canton_Init.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Initialisation complète du canton :
 *   - création des structures internes (CantonPeriph, Aig, Signal, Loco)
 *   - initialisation du capteur d’occupation (ConsoCourant)
 *   - configuration du MCP23017 (si présent)
 *
 * IMPORTANT :
 *   - aucune logique métier ici
 *   - aucune décision ferroviaire
 *   - uniquement la mise en place des objets internes
 */

#include "Canton.h"
#include "Config.h"
#include "debug_cc.h"
#include "ConsoCourant.h"

/* ============================================================================
 *  Singleton Canton (un seul canton par CC)
 * ==========================================================================*/
Canton *Canton::s_instance = nullptr;

/* ============================================================================
 *  Constructeur
 * ==========================================================================*/
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
    /* ------------------------------------------------------------------------
     *  Singleton
     * ------------------------------------------------------------------------ */
    s_instance = this;

    CC_LOG_TRACE("[Canton %u][Init][CC] Construction du canton\n", m_id);

    /* ------------------------------------------------------------------------
     *  Création des CantonPeriph (voisins)
     * ------------------------------------------------------------------------ */
    for (uint8_t i = 0; i < cantonPsize; i++)
    {
        cantonP[i] = new CantonPeriph();
        cantonP[i]->ID(UNUSED_ID); // par défaut : absent
    }

    /* ------------------------------------------------------------------------
     *  Création des aiguilles logiques
     * ------------------------------------------------------------------------ */
    for (uint8_t i = 0; i < aigSize; i++)
        aig[i] = new Aig();

    /* ------------------------------------------------------------------------
     *  Création des signaux (AH = 0, H = 1)
     * ------------------------------------------------------------------------ */
    signal[0] = new Signal(); // anti‑horaire
    signal[1] = new Signal(); // horaire

    /* ------------------------------------------------------------------------
     *  Capteurs ponctuels virtuels (EXCC → CC)
     *  → Pas de GPIO, pas de setup matériel
     * ------------------------------------------------------------------------ */

    /* ------------------------------------------------------------------------
     *  Loco interne
     * ------------------------------------------------------------------------ */
    loco = new Loco();

    /* ------------------------------------------------------------------------
     *  Capteur d’occupation (courant) — EXCC UART
     * ------------------------------------------------------------------------ */
    occupation = new ConsoCourant;
    occupation->setup(this);

    CC_LOG_TRACE("[Canton %u][Init][CC] CantonPeriph + Aig + Signal + Loco + ConsoCourant initialisés\n",
                 m_id);
}

/* ============================================================================
 *  Destructeur
 * ==========================================================================*/
Canton::~Canton()
{
    for (uint8_t i = 0; i < cantonPsize; i++)
        delete cantonP[i];

    for (uint8_t i = 0; i < aigSize; i++)
        delete aig[i];

    delete signal[0];
    delete signal[1];

    delete occupation;
    delete loco;

    CC_LOG_TRACE("[Canton %u][Init][CC] Destruction du canton\n", m_id);
}

/* ============================================================================
 *  Initialisation du MCP23017 (GPIO expander)
 * ==========================================================================*/
void Canton::initMCP()
{
    if (!mcp.begin_I2C(0x20))   // adresse par défaut
    {
        CC_LOG_ERROR("[Canton %u][Init][CC] MCP23017 introuvable !\n", m_id);
        return;
    }

    CC_LOG_INFO("[Canton %u][Init][CC] MCP23017 initialisé\n", m_id);
}

/* ============================================================================
 *  Identité du canton
 * ==========================================================================*/
void Canton::ID(uint16_t id)
{
    m_id = id;
}

uint16_t Canton::ID()
{
    return m_id;
}

/* ============================================================================
 *  Validation de la topologie (SP1_idx / SM1_idx)
 * ==========================================================================*/
void Canton::validateTopology()
{
    if (m_SP1_idx >= cantonPsize)
    {
        CC_LOG_WARN("[Canton %u][Init][CC] SP1_idx invalide (%u) → remis à 0\n",
                    m_id, m_SP1_idx);
        m_SP1_idx = 0;
    }

    if (m_SM1_idx >= cantonPsize)
    {
        CC_LOG_WARN("[Canton %u][Init][CC] SM1_idx invalide (%u) → remis à 0\n",
                    m_id, m_SM1_idx);
        m_SM1_idx = 0;
    }
}

/* ============================================================================
 *  Détection du sens de marche initial
 * ==========================================================================*/
void Canton::detectInitialDirection()
{
    bool ah = sensor[IDX_CAPT_ANTIHORAIRE].state();
    bool h  = sensor[IDX_CAPT_HORAIRE].state();

    if (ah && !h)
    {
        m_sensMarche = SensAntiHoraire;
        CC_LOG_INFO("[Canton %u][Init][CC] Sens initial : anti‑horaire\n", m_id);
    }
    else if (h && !ah)
    {
        m_sensMarche = SensHoraire;
        CC_LOG_INFO("[Canton %u][Init][CC] Sens initial : horaire\n", m_id);
    }
    else
    {
        CC_LOG_TRACE("[Canton %u][Init][CC] Sens initial indéterminé\n", m_id);
    }
}

/* ============================================================================
 *  logInitialState() — Diagnostic de démarrage
 * ==========================================================================*/
void Canton::logInitialState()
{
    CC_LOG_INFO("============================================================\n");
    CC_LOG_INFO("[Canton %u][Init][CC] Démarrage du canton\n", m_id);
    CC_LOG_INFO("Rôle ferroviaire : %u\n", m_role);
    CC_LOG_INFO("SP1_idx=%u | SM1_idx=%u\n", m_SP1_idx, m_SM1_idx);
    CC_LOG_INFO("Capteur AH=%d | H=%d\n",
                sensor[IDX_CAPT_ANTIHORAIRE].state(),
                sensor[IDX_CAPT_HORAIRE].state());
    CC_LOG_INFO("Occupation initiale : %d\n", m_busy);
    CC_LOG_INFO("============================================================\n");
}
