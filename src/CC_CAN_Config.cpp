/*
 * CanConfig.cpp — Gestion Canton 2026 (version CanUniversal)
 * ---------------------------------------------------------------------------
 * Initialise le bus CAN interne via CanUniversal :
 *   - configuration fournie par un CanConfigProvider
 *   - initialisation via CanInit
 *   - aucun accès direct à ACAN_ESP32
 */

#include "CC_CAN_Config.h"
#include "debug_cc.h"

#include <CanInit.h>
#include <CanConfig.h>

// ---------------------------------------------------------------------------
// Provider spécifique au CantonController
// ---------------------------------------------------------------------------
class CantonCanConfigProvider : public CanConfigProvider
{
public:
    uint8_t busCount() const override
    {
        return 1; // un seul bus pour le CC
    }

    const CanBusConfig &bus(uint8_t index) const override
    {
        static CanBusConfig cfg;

        if (index == 0)
        {
            static CanBusConfig cfg;

            cfg.enabled = true;
            cfg.speed = CAN_BITRATE;

            // TWAI interne ESP32
            cfg.tx_pin = PIN_CAN_TX;
            cfg.rx_pin = PIN_CAN_RX;

            // MCP2515 désactivé
            cfg.cs_pin = GPIO_NUM_NC;
            cfg.int_pin = GPIO_NUM_NC;
            cfg.sck_pin = GPIO_NUM_NC;
            cfg.mosi_pin = GPIO_NUM_NC;
            cfg.miso_pin = GPIO_NUM_NC;

            cfg.quartz = 0;
            cfg.tolerance = 0;
            cfg.loopback = false;
        }

        // Bus invalide par défaut
        static CanBusConfig invalid;
        return invalid;
    }
};

// ---------------------------------------------------------------------------
// Initialisation du CAN via CanUniversal
// ---------------------------------------------------------------------------
void CcCanConfig::setup()
{
    CC_LOG_INFO("[CanConfig][CC] Initialisation du CAN via CanUniversal\n");

    CantonCanConfigProvider provider;

    // Initialisation multi‑bus (ici : 1 bus)
    CanInit::begin(provider);

    CC_LOG_INFO("[CanConfig][CC] Initialisation terminée\n");
}
