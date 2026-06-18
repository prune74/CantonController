/*
 * EXCC_Link.cpp — Communication RS485 CC ↔ EXCC - Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Ce module gère la liaison série RS485 entre :
 *   - le Canton Controller (CC)
 *   - l’unique Extension Canton Controller (EXCC)
 *
 * Conception :
 *   → Un CC communique avec un seul EXCC
 *   → Le booster est intégré à l’EXCC
 *
 * Fonctionnalités :
 *   - supervision ONLINE / OFFLINE de l’EXCC
 *   - envoi périodique d’un PING
 *   - réception du PONG
 *   - réception des informations du booster :
 *        • tension
 *        • courant
 *        • état interne
 *   - envoi des configurations lors du passage ONLINE
 *   - envoi des seuils calibrés
 *   - commande ON/OFF du booster
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
static constexpr uint32_t kPingPeriodMs     = 500;   // période d’envoi du PING
static constexpr uint32_t kOfflineTimeoutMs = 2000;  // délai avant OFFLINE

// ---------------------------------------------------------------------------
// État interne de l’EXCC
// ---------------------------------------------------------------------------
struct ExccState
{
    bool     online;        // EXCC joignable ?
    uint32_t lastPongTime;  // horodatage du dernier PONG
};

static ExccState g_excc;

// ---------------------------------------------------------------------------
// Initialisation du lien RS485
// ---------------------------------------------------------------------------
void EXCC_Link::begin()
{
    CC_LOG_INFO("[EXCC][CC] Initialisation RS485...\n");

    CC_RS485::begin();

    g_excc.online       = false;
    g_excc.lastPongTime = 0;
}

// ---------------------------------------------------------------------------
// Envoi périodique du PING
// ---------------------------------------------------------------------------
static void envoyerPing()
{
    uint8_t frame[2] = {
        PROTO_SYNC_BYTE,
        PROTO_PING
    };

    CC_RS485::sendFrame(frame, sizeof(frame));
    CC_LOG_TRACE("[EXCC][CC] → PING\n");
}

void EXCC_Link::envoyerPingPeriodique()
{
    static uint32_t lastPing = 0;
    uint32_t now = millis();

    if (now - lastPing < kPingPeriodMs)
        return;

    lastPing = now;
    envoyerPing();
}

// ---------------------------------------------------------------------------
// Réception du PONG
// ---------------------------------------------------------------------------
void EXCC_Link::onPong()
{
    uint32_t now = millis();
    g_excc.lastPongTime = now;

    if (!g_excc.online)
    {
        g_excc.online = true;
        EXCC_Link::onExccOnline();
    }

    CC_LOG_TRACE("[EXCC][CC] PONG\n");
}

// ---------------------------------------------------------------------------
// Réception des informations Booster (PROTO_07)
// ---------------------------------------------------------------------------
void EXCC_Link::onBooster(uint8_t etat,
                          uint8_t courant,
                          uint8_t tension)
{
    Booster::onBooster(tension, courant, etat);

    CC_LOG_TRACE("[EXCC][CC] Booster : etat=%u  I=%u  U=%u\n",
                 etat, courant, tension);
}

// ---------------------------------------------------------------------------
// Détection OFFLINE
// ---------------------------------------------------------------------------
static void verifierTimeouts()
{
    uint32_t now = millis();

    if (!g_excc.online)
        return;

    if (now - g_excc.lastPongTime > kOfflineTimeoutMs)
    {
        g_excc.online = false;
        EXCC_Link::onExccOffline();
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
// Gestion ONLINE / OFFLINE
// ---------------------------------------------------------------------------
bool EXCC_Link::isOnline()
{
    return g_excc.online;
}

void EXCC_Link::onExccOnline()
{
    CC_LOG_INFO("[EXCC][CC] EXCC ONLINE\n");

    // Envoi des configurations courantes
    envoyerTopologieDepuisSettings();
    envoyerConfigurationSignauxDepuisSettings();
    envoyerConfigurationServosDepuisSettings();
    envoyerOccupationDepuisEtatCourant();
    envoyerAspectsDepuisEtatCourant();
    envoyerFeuxDepuisEtatCourant();

    // Envoi des seuils calibrés
    uint16_t libre  = Settings::boosterSeuilLibre();
    uint16_t occupe = Settings::boosterSeuilOccupe();

    EXCC_Link::envoyerSeuilsBooster(libre, occupe);
}

void EXCC_Link::onExccOffline()
{
    CC_LOG_WARN("[EXCC][CC] EXCC OFFLINE\n");
}

// ---------------------------------------------------------------------------
// Commande ON/OFF du booster
// ---------------------------------------------------------------------------
void EXCC_Link::envoyerBoosterPower(bool on)
{
    uint8_t frame[3] = {
        PROTO_SYNC_BYTE,
        PROTO_F5_BOOSTER_POWER,
        uint8_t(on ? 1 : 0)
    };

    CC_RS485::sendFrame(frame, sizeof(frame));

    CC_LOG_WARN("[EXCC][CC] → Booster = %s\n", on ? "ON" : "OFF");
}

// ---------------------------------------------------------------------------
// Demande de recalibration
// ---------------------------------------------------------------------------
void EXCC_Link::demanderRecalibration()
{
    uint8_t frame[2] = {
        PROTO_SYNC_BYTE,
        PROTO_F3_RECALIBRER_BOOSTER
    };

    CC_RS485::sendFrame(frame, sizeof(frame));

    CC_LOG_INFO("[EXCC][CC] → Demande recalibration\n");
}

// ---------------------------------------------------------------------------
// Envoi des seuils calibrés
// ---------------------------------------------------------------------------
void EXCC_Link::envoyerSeuilsBooster(uint16_t libre,
                                     uint16_t occupe)
{
    uint8_t frame[6] = {
        PROTO_SYNC_BYTE,
        PROTO_F4_SET_SEUILS,
        uint8_t(libre & 0xFF),
        uint8_t(libre >> 8),
        uint8_t(occupe & 0xFF),
        uint8_t(occupe >> 8)
    };

    CC_RS485::sendFrame(frame, sizeof(frame));

    CC_LOG_INFO("[EXCC][CC] → Seuils envoyés : libre=%u  occupé=%u\n",
                libre, occupe);
}
