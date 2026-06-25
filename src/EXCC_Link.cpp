/*
 * EXCC_Link.cpp — Communication CAN CC ↔ EXCC - Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Version CAN (remplace la liaison RS485 historique).
 *
 * Un CC communique avec un seul EXCC, lui-même associé à un Canton unique.
 * Le booster est intégré à l’EXCC.
 */

#include "EXCC_Link.h"
#include "Exploration.h"
#include "Booster.h"
#include "Settings.h"
#include "debug_cc.h"
#include "CC_CAN_EXCC.h"
#include "Canton.h"

// ---------------------------------------------------------------------------
// Paramètres internes
// ---------------------------------------------------------------------------
static constexpr uint32_t kPingPeriodMs = 500;      // période d’envoi du PING
static constexpr uint32_t kOfflineTimeoutMs = 2000; // délai avant OFFLINE

// ---------------------------------------------------------------------------
// État interne de l’EXCC
// ---------------------------------------------------------------------------
struct ExccState
{
    bool online;           // EXCC joignable ?
    uint32_t lastPongTime; // horodatage du dernier PONG
};

static ExccState g_excc;

// Canton associé à l’unique EXCC
static Canton *g_exccCanton = nullptr;

// ---------------------------------------------------------------------------
// Initialisation du lien CAN
// ---------------------------------------------------------------------------
void EXCC_Link::begin() // 🟢
{
    CC_LOG_INFO("[EXCC][CC] Initialisation lien CAN EXCC...\n");

    g_excc.online = false;
    g_excc.lastPongTime = 0;

    // Le CC communique avec l’EXCC via SON propre canton
    g_exccCanton = Exploration::getCanton();

    if (!g_exccCanton)
        CC_LOG_WARN("[EXCC][CC] Aucun canton associé à l’EXCC\n");
}

// ---------------------------------------------------------------------------
// Envoi périodique du PING (commande CAN CMD_CC_EXCC_PING)
// ---------------------------------------------------------------------------
static void envoyerPing() // 🟢
{
    if (!g_exccCanton)
        return;

    CC_CAN_EXCC::sendPing(g_exccCanton);
    CC_LOG_TRACE("[EXCC][CC] → PING (CAN)\n");
}

void EXCC_Link::envoyerPingPeriodique() // 🟢
{
    static uint32_t lastPing = 0;
    uint32_t now = millis();

    if (now - lastPing < kPingPeriodMs)
        return;

    lastPing = now;
    envoyerPing();
}

// ---------------------------------------------------------------------------
// Réception du PONG (appelé depuis CC_CAN_EXCC::handleEXCCCommand)
// ---------------------------------------------------------------------------
void EXCC_Link::onPong() // 🟢
{
    uint32_t now = millis();
    g_excc.lastPongTime = now;

    if (!g_excc.online)
    {
        g_excc.online = true;
        EXCC_Link::onExccOnline();
    }

    CC_LOG_TRACE("[EXCC][CC] PONG (CAN)\n");
}

// ---------------------------------------------------------------------------
// Réception des informations Booster (commande CAN CMD_EXCC_CC_BOOSTER_INFO)
// ---------------------------------------------------------------------------
void EXCC_Link::onBooster(uint8_t etat, uint8_t courant, uint8_t tension) // 🟢
{
    Booster::onBooster(tension, courant, etat);

    CC_LOG_TRACE("[EXCC][CC] Booster : etat=%u  I=%u  U=%u\n",
                 etat, courant, tension);
}

// ---------------------------------------------------------------------------
// Détection OFFLINE
// ---------------------------------------------------------------------------
static void verifierTimeouts() // 🟢
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
void EXCC_Link::loop() // 🟢
{
    envoyerPingPeriodique();
    verifierTimeouts();
}

// ---------------------------------------------------------------------------
// Gestion ONLINE / OFFLINE
// ---------------------------------------------------------------------------

void EXCC_Link::onExccOnline() // 🟢
{
    CC_LOG_INFO("[EXCC][CC] EXCC ONLINE (CAN)\n");

    // Envoi des configurations courantes
    CC_CAN_EXCC::sendConfigurationSignauxDepuisSettings();
    CC_CAN_EXCC::sendConfigurationServosDepuisSettings();
    CC_CAN_EXCC::sendAspectsDepuisEtatCourant();
    CC_CAN_EXCC::sendFeuxDepuisEtatCourant();
    CC_CAN_EXCC::sendAiguillesDepuisEtatCourant();

    // Envoi des seuils calibrés
    uint16_t libre = Settings::boosterSeuilLibre();
    uint16_t occupe = Settings::boosterSeuilOccupe();

    EXCC_Link::envoyerSeuilsBooster(libre, occupe);
}

void EXCC_Link::onExccOffline() // 🟢
{
    CC_LOG_WARN("[EXCC][CC] EXCC OFFLINE (CAN)\n");
}

// ---------------------------------------------------------------------------
// Commande ON/OFF du booster (commande CAN CMD_CC_EXCC_BOOSTER_POWER)
// ---------------------------------------------------------------------------
void EXCC_Link::envoyerBoosterPower(bool on) // 🟢
{
    if (!g_exccCanton)
        return;

    CC_CAN_EXCC::sendBoosterPower(g_exccCanton, on);

    CC_LOG_WARN("[EXCC][CC] → Booster = %s (CAN)\n", on ? "ON" : "OFF");
}

// ---------------------------------------------------------------------------
// Demande de recalibration (commande CAN CMD_EXCC_CC_CALIB_BOOSTER_INFO)
// ---------------------------------------------------------------------------
void EXCC_Link::demanderRecalibration() // 🟢
{
    if (!g_exccCanton)
        return;

    CC_CAN_EXCC::sendRecalibration(g_exccCanton);

    CC_LOG_INFO("[EXCC][CC] → Demande recalibration (CAN)\n");
}

// ---------------------------------------------------------------------------
// Envoi des seuils calibrés (commande CAN CMD_CC_EXCC_SET_SEUILS)
// ---------------------------------------------------------------------------
void EXCC_Link::envoyerSeuilsBooster(uint16_t libre, uint16_t occupe) // 🟢
{
    if (!g_exccCanton)
        return;

    CC_CAN_EXCC::sendSeuilsBooster(g_exccCanton, libre, occupe);

    CC_LOG_INFO("[EXCC][CC] → Seuils envoyés (CAN) : libre=%u  occupé=%u\n",
                libre, occupe);
}
