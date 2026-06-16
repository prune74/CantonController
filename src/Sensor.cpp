/*
 * Sensor.cpp — Gestion des capteurs ponctuels (EXCC → SA)
 * Version Exploration 2026
 */

#include "Sensor.h"
#include "Exploration_Protocol.h"
#include "Canton.h"
#include "debug_sa.h"

/*
 * Constructeur
 */
Sensor::Sensor() : m_state(false)
{
    SA_LOG_TRACE("[Sensor] Constructeur → état initial = LIBRE\n");
}

Sensor::~Sensor() {}

/*
 * setup() — paramètres conservés mais inutilisés
 */
void Sensor::setup(gpio_num_t pin, uint32_t tempo, uint8_t input)
{
    (void)pin;
    (void)tempo;
    (void)input;

    SA_LOG_INFO("[Sensor] setup() (virtuel, pas de GPIO)\n");
}

/*
 * Lecture / écriture interne
 */
bool Sensor::state()
{
    return m_state;
}

void Sensor::state(bool state)
{
    m_state = state;
    SA_LOG_TRACE("[Sensor] state() forcé → %s\n", state ? "ACTIF" : "LIBRE");
}

void Sensor::overrideState(bool state)
{
    m_state = state;
    SA_LOG_INFO("[Sensor] overrideState() → %s\n", state ? "ACTIF" : "LIBRE");
}

/*
 * ============================================================
 *  onPonctuelH() — Ponctuel côté H (Horaire)
 * ============================================================
 */
void Sensor::onPonctuelH(uint8_t code)
{
    bool actif = (code == PROTO_PONCT_H_ACTIVE);

    Canton *canton = Canton::s_instance;
    if (!canton)
    {
        SA_LOG_ERROR("[Sensor] Canton::s_instance nul\n");
        return;
    }

    canton->overrideCapteur(SensHoraire, actif);

    SA_LOG_TRACE("[Sensor] H = %s\n", actif ? "ACTIF" : "LIBRE");
}

/*
 * ============================================================
 *  onPonctuelAH() — Ponctuel côté AH (Anti‑Horaire)
 * ============================================================
 */
void Sensor::onPonctuelAH(uint8_t code)
{
    bool actif = (code == PROTO_PONCT_AH_ACTIVE);

    Canton *canton = Canton::s_instance;
    if (!canton)
    {
        SA_LOG_ERROR("[Sensor] Canton::s_instance nul\n");
        return;
    }

    canton->overrideCapteur(SensAntiHoraire, actif);

    SA_LOG_TRACE("[Sensor] AH = %s\n", actif ? "ACTIF" : "LIBRE");
}
