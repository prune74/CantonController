/* 
 * Sensor.cpp - Gestion des capteurs virtuels (EXSA → SA)
 * Version 2026 — Option A (plus de capteur physique)
 *
 * Ce module remplace totalement les capteurs physiques par des capteurs
 * virtuels transmis via UART par l’EXSA.
 *
 * Le protocole 2026 repose sur :
 *   - un octet de synchronisation (PROTO_SYNC_BYTE)
 *   - un octet de type (ici PROTO_03_PONCTUEL)
 *   - un octet de valeur (H_ACTIVE, AH_ACTIVE, etc.)
 *
 * Le SA ne lit plus aucune GPIO : tout passe par l’UART.
 */

#include "Sensor.h"
#include "Settings.h"
#include "SA_EXSA_Protocol.h"
#include "Config.h"
#include "debug_sa.h"

/*
 * Constructeur
 * ---------------------------------------------------------------------------
 * Initialise l’état interne du capteur à false (libre).
 */
Sensor::Sensor() : m_state(false)
{
    SA_LOG_TRACE("[Sensor] Constructeur → état initial = LIBRE\n");
}

Sensor::~Sensor() {}

/*
 * setup()
 * ---------------------------------------------------------------------------
 * Paramètres conservés pour compatibilité API, mais inutilisés en 2026.
 *
 * Le capteur est désormais 100% virtuel :
 *   → on lance une tâche FreeRTOS dédiée à la lecture UART.
 */
void Sensor::setup(gpio_num_t pin, uint32_t tempo, byte input)
{
    (void)pin;
    (void)tempo;
    (void)input;

    SA_LOG_INFO("[Sensor] setup() → lancement de la tâche UART\n");

    xTaskCreate(this->uartLoop, "SensorUART", 2048, this, 1, NULL);
}

/*
 * uartLoop()
 * ---------------------------------------------------------------------------
 * Boucle FreeRTOS dédiée à la lecture du protocole EXSA.
 *
 * Format attendu :
 *   [0] PROTO_SYNC_BYTE
 *   [1] PROTO_03_PONCTUEL
 *   [2] code capteur :
 *         - PROTO_PONCT_H_ACTIVE
 *         - PROTO_PONCT_H_INACTIVE
 *         - PROTO_PONCT_AH_ACTIVE
 *         - PROTO_PONCT_AH_INACTIVE
 *
 * Cette boucle tourne toutes les 5 ms.
 */
void IRAM_ATTR Sensor::uartLoop(void* p)
{
    Sensor* self = static_cast<Sensor*>(p);
    HardwareSerial& uart = Settings::uart();
    static uint8_t step = 0;
    static uint8_t buffer[3];

    SA_LOG_TRACE("[Sensor] uartLoop() démarrée\n");

    for (;;)
    {
        if (uart.available())
        {
            uint8_t byte = uart.read();

            switch (step)
            {
                case 0:
                    if (byte == PROTO_SYNC_BYTE)
                    {
                        buffer[0] = byte;
                        step = 1;
                    }
                    break;

                case 1:
                    buffer[1] = byte;
                    step = 2;
                    break;

                case 2:
                    buffer[2] = byte;
                    step = 0;

                    // 🔥 Nouveau protocole : trame ponctuelle = 0x03
                    if (buffer[1] == PROTO_03_PONCTUEL)
                    {
                        switch (buffer[2])
                        {
                            case PROTO_PONCT_H_ACTIVE:
                            case PROTO_PONCT_AH_ACTIVE:
                                self->overrideState(true);
                                SA_LOG_INFO("[Sensor] Trame %02X → ACTIF\n", buffer[2]);
                                break;

                            case PROTO_PONCT_H_INACTIVE:
                            case PROTO_PONCT_AH_INACTIVE:
                                self->overrideState(false);
                                SA_LOG_INFO("[Sensor] Trame %02X → LIBRE\n", buffer[2]);
                                break;

                            default:
                                SA_LOG_WARN("[Sensor] Trame inconnue %02X ignorée\n", buffer[2]);
                                break;
                        }
                    }
                    else
                    {
                        SA_LOG_TRACE("[Sensor] Type de trame ignoré : %02X\n", buffer[1]);
                    }
                    break;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

/*
 * state()
 * ---------------------------------------------------------------------------
 * Retourne l’état interne du capteur.
 */
bool Sensor::state()
{
    return m_state;
}

/*
 * state(bool)
 * ---------------------------------------------------------------------------
 * Modifie l’état interne du capteur (rarement utilisé).
 */
void Sensor::state(bool state)
{
    m_state = state;
    SA_LOG_TRACE("[Sensor] state() forcé → %s\n", state ? "ACTIF" : "LIBRE");
}

/*
 * overrideState()
 * ---------------------------------------------------------------------------
 * Méthode utilisée par uartLoop() pour mettre à jour l’état du capteur.
 *
 * Cette méthode est la seule voie officielle pour modifier m_state.
 */
void Sensor::overrideState(bool state)
{
    m_state = state;

    SA_LOG_INFO("[Sensor] État mis à jour : %s\n", state ? "ACTIF" : "LIBRE");
}
