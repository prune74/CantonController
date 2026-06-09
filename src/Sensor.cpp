/*
 * Sensor.cpp — Gestion des capteurs ponctuels (EXSA → SA)
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
 * onPonctuel() — callback appelé par SA_UartRx
 *
 * index_exsa :
 *   0 = Horaire
 *   1 = AntiHoraire
 */
void Sensor::onPonctuel(uint8_t index_exsa, uint8_t code)
{
    bool actif = false;

    switch (code)
    {
    case PROTO_PONCT_H_ACTIVE:
    case PROTO_PONCT_AH_ACTIVE:
        actif = true;
        break;

    case PROTO_PONCT_H_INACTIVE:
    case PROTO_PONCT_AH_INACTIVE:
        actif = false;
        break;

    default:
        SA_LOG_WARN("[Sensor] Code ponctuel inconnu : %02X\n", code);
        return;
    }

    // ============================================================
    // Récupération du Canton unique du SA
    // ============================================================
    Canton *canton = Canton::s_instance;
    if (!canton)
    {
        SA_LOG_ERROR("[Sensor] Canton::s_instance nul\n");
        return;
    }

    // ============================================================
    // Conversion EXSA → sens ferroviaire
    // ============================================================
    SensDeMarche sens =
        (index_exsa == 0) ? SensHoraire : SensAntiHoraire;

    // ============================================================
    // Mise à jour du capteur via l’API publique du Canton
    // ============================================================
    canton->overrideCapteur(sens, actif);

    SA_LOG_TRACE("[Sensor] EXSA=%u → capteur %s = %s\n",
                 index_exsa,
                 (sens == SensHoraire) ? "H" : "AH",
                 actif ? "ACTIF" : "LIBRE");
}
