/*
 * EXCC_Link.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Couche de communication RS485 entre :
 *   - le Canton Controller (CC)
 *   - l’Extension Canton Controller (EXCC)
 *
 * Rôle :
 *   - supervision ONLINE / OFFLINE des EXCC
 *   - envoi périodique des PING (PROTO_PING)
 *   - réception des PONG (PROTO_PONG)
 *   - gestion du booster (tension, courant, état, présence)
 *   - envoi des configurations lors d’un ONLINE :
 *        • topologie
 *        • signaux
 *        • servos
 *        • occupation
 *        • aspects SNCF
 *        • feux directionnels
 *   - envoi des seuils calibrés (F4)
 *   - commande ON/OFF du booster (F5)
 */

#include "EXCC_Link.h"
#include "Exploration_Protocol.h"
#include "SatTopologieUART.h"
#include "CC_RS485.h"
#include "Booster.h"
#include "Settings.h"
#include "debug_cc.h"

// ---------------------------------------------------------------------------
// Paramètres internes
// ---------------------------------------------------------------------------
static constexpr uint8_t  kExccCount        = 2;
static constexpr uint32_t kPingPeriodMs     = 500;
static constexpr uint32_t kOfflineTimeoutMs = 2000;

// ---------------------------------------------------------------------------
// État interne EXCC
// ---------------------------------------------------------------------------
struct ExccState
{
    bool online;
    uint32_t lastPongTime;
};

static ExccState g_excc[kExccCount];
static int8_t g_exccBoosterIndex = -1;

// ---------------------------------------------------------------------------
// Initialisation
// ---------------------------------------------------------------------------
void EXCC_Link::begin()
{
    CC_LOG_INFO("[EXCC][CC] Initialisation RS485...\n");

    CC_RS485::begin();

    for (uint8_t i = 0; i < kExccCount; ++i)
    {
        g_excc[i].online = false;
        g_excc[i].lastPongTime = 0;
    }

    g_exccBoosterIndex = -1;
}

// ---------------------------------------------------------------------------
// PING périodique
// ---------------------------------------------------------------------------
static void envoyerPing(uint8_t index)
{
    uint8_t frame[3] = {
        PROTO_SYNC_BYTE,
        PROTO_PING,
        index
    };

    CC_RS485::sendFrame(frame, sizeof(frame));
    CC_LOG_TRACE("[EXCC][CC] → PING EXCC %u\n", index);
}

void EXCC_Link::envoyerPingPeriodique()
{
    static uint32_t lastPing = 0;
    uint32_t now = millis();

    if (now - lastPing < kPingPeriodMs)
        return;

    lastPing = now;

    for (uint8_t i = 0; i < kExccCount; ++i)
        envoyerPing(i);
}

// ---------------------------------------------------------------------------
// PONG reçu
// ---------------------------------------------------------------------------
void EXCC_Link::onPong(uint8_t index)
{
    if (index >= kExccCount)
    {
        CC_LOG_WARN("[EXCC][CC] PONG index invalide : %u\n", index);
        return;
    }

    uint32_t now = millis();
    g_excc[index].lastPongTime = now;

    if (!g_excc[index].online)
    {
        g_excc[index].online = true;
        EXCC_Link::onExccOnline(index);
    }

    CC_LOG_TRACE("[EXCC][CC] PONG EXCC %u\n", index);
}

// ---------------------------------------------------------------------------
// BOOSTER (PROTO_07)
// ---------------------------------------------------------------------------
void EXCC_Link::onBooster(uint8_t index,
                          uint8_t etat,
                          uint8_t courant,
                          uint8_t tension,
                          uint8_t present)
{
    if (index >= kExccCount)
    {
        CC_LOG_WARN("[EXCC][CC] BOOSTER index invalide : %u\n", index);
        return;
    }

    Booster::onBooster(index, tension, courant, etat, present);

    if (present == 1)
    {
        g_exccBoosterIndex = index;
        CC_LOG_INFO("[EXCC][CC] Booster présent sur EXCC %u\n", index);
    }

    CC_LOG_TRACE("[EXCC][CC] Booster EXCC %u : etat=%u courant=%u tension=%u present=%u\n",
                 index, etat, courant, tension, present);
}

// ---------------------------------------------------------------------------
// Détection OFFLINE
// ---------------------------------------------------------------------------
static void verifierTimeouts()
{
    uint32_t now = millis();

    for (uint8_t i = 0; i < kExccCount; ++i)
    {
        if (!g_excc[i].online)
            continue;

        if (now - g_excc[i].lastPongTime > kOfflineTimeoutMs)
        {
            g_excc[i].online = false;
            EXCC_Link::onExccOffline(i);
        }
    }
}

// ---------------------------------------------------------------------------
// Boucle principale
// ---------------------------------------------------------------------------
void EXCC_Link::loop()
{
    envoyerPingPeriodique();
    verifierTimeouts();
}

// ---------------------------------------------------------------------------
// ONLINE / OFFLINE
// ---------------------------------------------------------------------------
bool EXCC_Link::isOnline(uint8_t index)
{
    if (index >= kExccCount)
        return false;

    return g_excc[index].online;
}

void EXCC_Link::onExccOnline(uint8_t index)
{
    CC_LOG_INFO("[EXCC][CC] EXCC %u ONLINE\n", index);

    // Envoi des configurations
    envoyerTopologieDepuisSettings();
    envoyerConfigurationSignauxDepuisSettings();
    envoyerConfigurationServosDepuisSettings();
    envoyerOccupationDepuisEtatCourant();
    envoyerAspectsDepuisEtatCourant();
    envoyerFeuxDepuisEtatCourant();

    // Envoi des seuils calibrés (F4)
    uint16_t libre  = Settings::boosterSeuilLibre();
    uint16_t occupe = Settings::boosterSeuilOccupe();

    EXCC_Link::envoyerSeuilsBooster(index, libre, occupe);
}

void EXCC_Link::onExccOffline(uint8_t index)
{
    CC_LOG_WARN("[EXCC][CC] EXCC %u OFFLINE\n", index);
}

// ---------------------------------------------------------------------------
// Booster ON/OFF (F5)
// ---------------------------------------------------------------------------
void EXCC_Link::envoyerBoosterPower(uint8_t index, bool on)
{
    if (index >= kExccCount)
        return;

    uint8_t frame[4] = {
        PROTO_SYNC_BYTE,
        PROTO_F5_BOOSTER_POWER,
        index,
        uint8_t(on ? 1 : 0)
    };

    CC_RS485::sendFrame(frame, sizeof(frame));

    CC_LOG_WARN("[EXCC][CC] → Booster EXCC %u = %s\n",
                index, on ? "ON" : "OFF");
}

// ---------------------------------------------------------------------------
// F3 — Demande recalibration booster
// ---------------------------------------------------------------------------
void EXCC_Link::demanderRecalibration(uint8_t index)
{
    if (index >= kExccCount)
        return;

    uint8_t frame[3] = {
        PROTO_SYNC_BYTE,
        PROTO_F3_RECALIBRER_BOOSTER,
        index
    };

    CC_RS485::sendFrame(frame, sizeof(frame));

    CC_LOG_INFO("[EXCC][CC] → Demande recalibration EXCC %u (F3)\n", index);
}

// ---------------------------------------------------------------------------
// F4 — Envoi seuils calibrés
// ---------------------------------------------------------------------------
void EXCC_Link::envoyerSeuilsBooster(uint8_t index,
                                     uint16_t libre,
                                     uint16_t occupe)
{
    if (index >= kExccCount)
        return;

    uint8_t frame[6] = {
        PROTO_SYNC_BYTE,
        PROTO_F4_SET_SEUILS,
        uint8_t(libre & 0xFF),
        uint8_t(libre >> 8),
        uint8_t(occupe & 0xFF),
        uint8_t(occupe >> 8)
    };

    CC_RS485::sendFrame(frame, sizeof(frame));

    CC_LOG_INFO("[EXCC][CC] → Seuils EXCC %u : libre=%u occupe=%u (F4)\n",
                index, libre, occupe);
}

// ---------------------------------------------------------------------------
// Booster porteur
// ---------------------------------------------------------------------------
int8_t EXCC_Link::getBoosterExccIndex()
{
    return g_exccBoosterIndex;
}
