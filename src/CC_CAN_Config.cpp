/*
 * CanConfig.cpp — Gestion Canton 2026 (version CanUniversal)
 * ---------------------------------------------------------------------------
 * CC = 2 bus CAN :
 *   - Bus 0 : TWAI interne ESP32 → réseau CantonController
 *   - Bus 1 : MCP2515 externe   → lien EXCC
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
        return 2; // 🟢 Deux bus CAN maintenant
    }

    const CanBusConfig &bus(uint8_t index) const override
    {
        static CanBusConfig cfg;

        switch (index)
        {
        case 0: // 🟢 Bus Gestion Canton (TWAI interne)
            cfg.enabled   = true;
            cfg.speed     = CAN_BITRATE;

            cfg.tx_pin    = PIN_CAN_TX;
            cfg.rx_pin    = PIN_CAN_RX;

            cfg.cs_pin    = GPIO_NUM_NC;
            cfg.int_pin   = GPIO_NUM_NC;
            cfg.sck_pin   = GPIO_NUM_NC;
            cfg.mosi_pin  = GPIO_NUM_NC;
            cfg.miso_pin  = GPIO_NUM_NC;

            cfg.quartz    = 0;
            cfg.tolerance = 0;
            cfg.loopback  = false;
            return cfg;

        case 1: // 🟢 Bus EXCC (MCP2515)
            cfg.enabled   = true;
            cfg.speed     = CAN_BITRATE_MCP2515;

            cfg.tx_pin    = GPIO_NUM_NC;
            cfg.rx_pin    = GPIO_NUM_NC;

            cfg.cs_pin    = PIN_EXCC_CS;
            cfg.int_pin   = PIN_EXCC_INT;

            cfg.sck_pin   = PIN_EXCC_SCK;
            cfg.mosi_pin  = PIN_EXCC_MOSI;
            cfg.miso_pin  = PIN_EXCC_MISO;

            cfg.quartz    = QUARTZ_MCP2515;
            cfg.tolerance = 0;
            cfg.loopback  = false;
            return cfg;
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
    CC_LOG_INFO("[CanConfig][CC] Initialisation du CAN via CanUniversal (2 bus)\n");

    CantonCanConfigProvider provider;

    // Initialisation multi‑bus (ici : 2 bus)
    CanInit::begin(provider);

    CC_LOG_INFO("[CanConfig][CC] Initialisation terminée\n");
}
