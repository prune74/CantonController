/*
 * Sensor.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Gestion des capteurs ponctuels remontés par l’Extension Canton Controller
 * (EXCC) vers le Canton Controller (CC).
 *
 * Rôle :
 *   - représenter un capteur ponctuel (virtuel, pas de GPIO local)
 *   - stocker son état interne (actif / libre)
 *   - appliquer les notifications EXCC :
 *       • onPonctuelH()  → capteur côté Horaire (H)
 *       • onPonctuelAH() → capteur côté Anti‑Horaire (AH)
 *
 * Ce module ne lit aucun GPIO : les capteurs physiques sont gérés
 * par l’EXCC, qui envoie les codes PROTO_PONCT_*.
 */

#include "Sensor.h"
#include "Exploration_Protocol.h"
#include "Canton.h"
#include "debug_cc.h"

// ---------------------------------------------------------------------------
// Constructeur
// ---------------------------------------------------------------------------
Sensor::Sensor() : m_state(false)
{
    CC_LOG_TRACE("[Sensor][CC] Constructeur → état initial = LIBRE\n");
}

Sensor::~Sensor() {}

// ---------------------------------------------------------------------------
// setup() — paramètres conservés mais inutilisés (capteur virtuel)
// ---------------------------------------------------------------------------
void Sensor::setup(gpio_num_t pin, uint32_t tempo, uint8_t input)
{
    (void)pin;
    (void)tempo;
    (void)input;

    CC_LOG_INFO("[Sensor][CC] setup() (virtuel, pas de GPIO)\n");
}

// ---------------------------------------------------------------------------
// Lecture / écriture interne
// ---------------------------------------------------------------------------
bool Sensor::state()
{
    return m_state;
}

void Sensor::state(bool state)
{
    m_state = state;
    CC_LOG_TRACE("[Sensor][CC] state() forcé → %s\n", state ? "ACTIF" : "LIBRE");
}

void Sensor::overrideState(bool state)
{
    m_state = state;
    CC_LOG_INFO("[Sensor][CC] overrideState() → %s\n", state ? "ACTIF" : "LIBRE");
}

// ---------------------------------------------------------------------------
// onPonctuelH() — Notification EXCC côté Horaire (H)
// ---------------------------------------------------------------------------
void Sensor::onPonctuelH(uint8_t code)
{
    bool actif = (code == PROTO_PONCT_H_ACTIVE);

    Canton *canton = Canton::s_instance;
    if (!canton)
    {
        CC_LOG_ERROR("[Sensor][CC] Canton::s_instance nul\n");
        return;
    }

    canton->overrideCapteur(SensHoraire, actif);

    CC_LOG_TRACE("[Sensor][CC] H = %s\n", actif ? "ACTIF" : "LIBRE");
}

// ---------------------------------------------------------------------------
// onPonctuelAH() — Notification EXCC côté Anti‑Horaire (AH)
// ---------------------------------------------------------------------------
void Sensor::onPonctuelAH(uint8_t code)
{
    bool actif = (code == PROTO_PONCT_AH_ACTIVE);

    Canton *canton = Canton::s_instance;
    if (!canton)
    {
        CC_LOG_ERROR("[Sensor][CC] Canton::s_instance nul\n");
        return;
    }

    canton->overrideCapteur(SensAntiHoraire, actif);

    CC_LOG_TRACE("[Sensor][CC] AH = %s\n", actif ? "ACTIF" : "LIBRE");
}
