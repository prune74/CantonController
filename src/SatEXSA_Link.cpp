#include "SatEXSA_Link.h"
#include "Discovery_Protocol.h"
#include "SatTopologieUART.h"
#include "SA_RS485.h"
#include "Booster.h"
#include "Settings.h"
#include "debug_sa.h"

static constexpr uint8_t kExsaCount = 2;
static constexpr uint32_t kPingPeriodMs = 500;
static constexpr uint32_t kOfflineTimeoutMs = 2000;

/* ============================================================
   État interne EXSA
   ============================================================ */
struct ExsaState
{
    bool online;
    uint32_t lastPongTime;
};

static ExsaState g_exsa[kExsaCount];
static int8_t g_exsaBoosterIndex = -1;

/* ============================================================
   Initialisation
   ============================================================ */
void SatEXSA_Link::begin()
{
    SA_LOG_INFO("[SatEXSA_Link] Initialisation RS485...\n");

    SA_RS485::begin();

    for (uint8_t i = 0; i < kExsaCount; ++i)
    {
        g_exsa[i].online = false;
        g_exsa[i].lastPongTime = 0;
    }

    g_exsaBoosterIndex = -1;
}

/* ============================================================
   PING périodique
   ============================================================ */
static void envoyerPing(uint8_t index)
{
    uint8_t frame[3] = {
        PROTO_SYNC_BYTE,
        PROTO_PING,
        index};

    SA_RS485::sendFrame(frame, sizeof(frame));
    SA_LOG_TRACE("[SatEXSA_Link] → PING EXSA %u\n", index);
}

void SatEXSA_Link::envoyerPingPeriodique()
{
    static uint32_t lastPing = 0;
    uint32_t now = millis();

    if (now - lastPing < kPingPeriodMs)
        return;

    lastPing = now;

    for (uint8_t i = 0; i < kExsaCount; ++i)
        envoyerPing(i);
}

/* ============================================================
   PONG reçu
   ============================================================ */
void SatEXSA_Link::onPong(uint8_t index)
{
    if (index >= kExsaCount)
    {
        SA_LOG_WARN("[SatEXSA_Link] PONG index invalide : %u\n", index);
        return;
    }

    uint32_t now = millis();
    g_exsa[index].lastPongTime = now;

    if (!g_exsa[index].online)
    {
        g_exsa[index].online = true;
        SatEXSA_Link::onExsaOnline(index);
    }

    SA_LOG_TRACE("[SatEXSA_Link] PONG EXSA %u\n", index);
}

/* ============================================================
   BOOSTER (PROTO_07)
   ============================================================ */
void SatEXSA_Link::onBooster(uint8_t index,
                             uint8_t etat,
                             uint8_t courant,
                             uint8_t tension,
                             uint8_t present)
{
    if (index >= kExsaCount)
    {
        SA_LOG_WARN("[SatEXSA_Link] BOOSTER index invalide : %u\n", index);
        return;
    }

    Booster::onBooster(index, tension, courant, etat, present);

    if (present == 1)
    {
        g_exsaBoosterIndex = index;
        SA_LOG_INFO("[SatEXSA_Link] Booster présent sur EXSA %u\n", index);
    }

    SA_LOG_TRACE("[SatEXSA_Link] Booster EXSA %u : etat=%u courant=%u tension=%u present=%u\n",
                 index, etat, courant, tension, present);
}

/* ============================================================
   Détection OFFLINE
   ============================================================ */
static void verifierTimeouts()
{
    uint32_t now = millis();

    for (uint8_t i = 0; i < kExsaCount; ++i)
    {
        if (!g_exsa[i].online)
            continue;

        if (now - g_exsa[i].lastPongTime > kOfflineTimeoutMs)
        {
            g_exsa[i].online = false;
            SatEXSA_Link::onExsaOffline(i);
        }
    }
}

/* ============================================================
   Boucle principale
   ============================================================ */
void SatEXSA_Link::loop()
{
    envoyerPingPeriodique();
    verifierTimeouts();
}

/* ============================================================
   ONLINE / OFFLINE
   ============================================================ */
bool SatEXSA_Link::isOnline(uint8_t index)
{
    if (index >= kExsaCount)
        return false;

    return g_exsa[index].online;
}

void SatEXSA_Link::onExsaOnline(uint8_t index)
{
    SA_LOG_INFO("[SatEXSA_Link] EXSA %u ONLINE\n", index);

    /* Envoi des configurations */
    envoyerTopologieDepuisSettings();
    envoyerConfigurationSignauxDepuisSettings();
    envoyerConfigurationServosDepuisSettings();
    envoyerOccupationDepuisEtatCourant();
    envoyerAspectsDepuisEtatCourant();
    envoyerFeuxDepuisEtatCourant();

    /* Envoi des seuils calibrés (F4) */
    uint16_t libre = Settings::boosterSeuilLibre();
    uint16_t occupe = Settings::boosterSeuilOccupe();

    SatEXSA_Link::envoyerSeuilsBooster(index, libre, occupe);
}

void SatEXSA_Link::onExsaOffline(uint8_t index)
{
    SA_LOG_WARN("[SatEXSA_Link] EXSA %u OFFLINE\n", index);
}

/* ============================================================
   Booster ON/OFF (F5)
   ============================================================ */
void SatEXSA_Link::envoyerBoosterPower(uint8_t index, bool on)
{
    if (index >= kExsaCount)
        return;

    uint8_t value = on ? 1 : 0;

    uint8_t frame[4] = {
        PROTO_SYNC_BYTE,
        PROTO_F5_BOOSTER_POWER,
        index,
        value};

    SA_RS485::sendFrame(frame, sizeof(frame));

    SA_LOG_WARN("[SatEXSA_Link] → Booster EXSA %u = %s\n",
                index, on ? "ON" : "OFF");
}

/* ============================================================
   F3 — Demande recalibration booster
   ============================================================ */
void SatEXSA_Link::demanderRecalibration(uint8_t index)
{
    if (index >= kExsaCount)
        return;

    uint8_t frame[3] = {
        PROTO_SYNC_BYTE,
        PROTO_F3_RECALIBRER_BOOSTER,
        index};

    SA_RS485::sendFrame(frame, sizeof(frame));

    SA_LOG_INFO("[SatEXSA_Link] → Demande recalibration EXSA %u (F3)\n", index);
}

/* ============================================================
   F4 — Envoi seuils calibrés
   ============================================================ */
void SatEXSA_Link::envoyerSeuilsBooster(uint8_t index,
                                        uint16_t libre,
                                        uint16_t occupe)
{
    if (index >= kExsaCount)
        return;

    uint8_t frame[6] = {
        PROTO_SYNC_BYTE,
        PROTO_F4_SET_SEUILS,
        uint8_t(libre & 0xFF),
        uint8_t(libre >> 8),
        uint8_t(occupe & 0xFF),
        uint8_t(occupe >> 8)};

    SA_RS485::sendFrame(frame, sizeof(frame));

    SA_LOG_INFO("[SatEXSA_Link] → Seuils EXSA %u : libre=%u occupe=%u (F4)\n",
                index, libre, occupe);
}

/* ============================================================
   Booster porteur
   ============================================================ */
int8_t SatEXSA_Link::getBoosterExsaIndex()
{
    return g_exsaBoosterIndex;
}
