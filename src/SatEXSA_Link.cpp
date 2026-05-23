/*
 * SatEXSA_Link.cpp
 * ------------------------------------------------------------
 * Supervision des modules EXSA via PING/PONG.
 *
 * Cette couche est purement "communication" :
 *   - PING SA → EXSA (PROTO_PING)
 *   - PONG EXSA → SA (PROTO_PONG)
 *   - Suivi ONLINE / OFFLINE
 *
 * Elle ne dépend pas de la logique métier des cantons.
 */

#include "SatEXSA_Link.h"
#include "SA_EXSA_Protocol.h"
#include "SatTopologieUART.h"   // envoyerTopologieDepuisSettings(), envoyerConfigurationSignauxDepuisSettings(), envoyerConfigurationServosDepuisSettings(), envoyerOccupationDepuisEtatCourant(), envoyerAspectsDepuisEtatCourant()
#include <Arduino.h>
#include "Config.h"
#include "debug_sa.h"

extern HardwareSerial Serial1;  // UART commun aux EXSA

/* ============================================================
   Paramètres de supervision
   ============================================================ */

// Nombre d’EXSA sur le bus (H et AH)
static constexpr uint8_t kExsaCount = 2;

// Période d’envoi des PING (en ms)
static constexpr uint32_t kPingPeriodMs = 500;

// Délai avant de considérer un EXSA comme OFFLINE (en ms)
static constexpr uint32_t kOfflineTimeoutMs = 2000;

/* ============================================================
   État interne par EXSA
   ============================================================ */

struct ExsaState {
    bool     online;        // true = on reçoit régulièrement des PONG
    uint32_t lastPongTime;  // millis() du dernier PONG reçu
};

static ExsaState g_exsa[kExsaCount];

/* ============================================================
   Initialisation
   ============================================================ */

void SatEXSA_Link::begin()
{
    SA_LOG_INFO("[SatEXSA_Link] Initialisation de la supervision EXSA...\n");

    for (uint8_t i = 0; i < kExsaCount; ++i) {
        g_exsa[i].online       = false;
        g_exsa[i].lastPongTime = 0;
    }

    SA_LOG_TRACE("[SatEXSA_Link] États EXSA initialisés à OFFLINE\n");
}

/* ============================================================
   Helpers internes : envoi PING
   ============================================================ */

static void envoyerPing(uint8_t index)
{
    // index = 0 (H) ou 1 (AH)
    Serial1.write(PROTO_SYNC_BYTE);
    Serial1.write(PROTO_PING);
    Serial1.write(index);

    SA_LOG_TRACE("[SatEXSA_Link] → PING envoyé à EXSA %u\n", index);
}

static void envoyerPingPeriodique()
{
    static uint32_t lastPing = 0;
    const uint32_t now = millis();

    if (now - lastPing < kPingPeriodMs)
        return;

    lastPing = now;

    // PING pour chaque EXSA (0 = H, 1 = AH)
    for (uint8_t i = 0; i < kExsaCount; ++i)
        envoyerPing(i);
}

/* ============================================================
   Helpers internes : lecture PONG
   ============================================================ */

/*
 * Mini-parser 3 octets :
 *   [SYNC][OPCODE][DATA]
 *
 * On ne s’intéresse qu’à :
 *   [0xAA][PROTO_PONG][index]
 *
 * Les autres trames EXSA → SA sont traitées ailleurs.
 */
static void lireReponsesEXSA()
{
    static uint8_t state  = 0;  // 0 = SYNC, 1 = OPCODE, 2 = DATA
    static uint8_t opcode = 0;
    static uint8_t data   = 0;

    while (Serial1.available()) {
        const uint8_t b = Serial1.read();

        switch (state) {

            case 0: // attente SYNC
                if (b == PROTO_SYNC_BYTE)
                    state = 1;
                break;

            case 1: // lecture opcode
                opcode = b;
                state  = 2;
                break;

            case 2: // lecture DATA
                data = b;

                if (opcode == PROTO_PONG) {
                    const uint8_t index = data;

                    SA_LOG_TRACE("[SatEXSA_Link] ← PONG reçu de EXSA %u\n", index);

                    if (index < kExsaCount) {
                        g_exsa[index].lastPongTime = millis();

                        if (!g_exsa[index].online) {
                            g_exsa[index].online = true;
                            SatEXSA_Link::onExsaOnline(index);
                        }
                    }
                }

                state = 0;
                break;

            default:
                state = 0;
                break;
        }
    }
}

/* ============================================================
   Helpers internes : détection OFFLINE
   ============================================================ */

static void verifierTimeouts()
{
    const uint32_t now = millis();

    for (uint8_t i = 0; i < kExsaCount; ++i) {

        if (!g_exsa[i].online)
            continue;

        // Si plus de kOfflineTimeoutMs sans PONG → OFFLINE
        if (now - g_exsa[i].lastPongTime > kOfflineTimeoutMs) {

            SA_LOG_WARN("[SatEXSA_Link] EXSA %u OFFLINE (timeout)\n", i);

            g_exsa[i].online = false;
            SatEXSA_Link::onExsaOffline(i);
        }
    }
}

/* ============================================================
   Boucle principale de supervision
   ============================================================ */

void SatEXSA_Link::loop()
{
    envoyerPingPeriodique();
    lireReponsesEXSA();
    verifierTimeouts();
}

/* ============================================================
   Hooks ONLINE / OFFLINE
   ============================================================ */

bool SatEXSA_Link::isOnline(uint8_t index)
{
    if (index >= kExsaCount)
        return false;

    return g_exsa[index].online;
}

void SatEXSA_Link::onExsaOnline(uint8_t index)
{
    SA_LOG_INFO("[SatEXSA_Link] EXSA %u ONLINE (PONG reçu)\n", index);

    /*
     * Ici, tu peux resynchroniser l’EXSA fraîchement rebooté.
     *
     * Idée générale :
     *   1) Renvoyer la topologie (E4)
     *   2) Renvoyer la config signaux (E5)
     *   3) Renvoyer la config servos (F1)
     *   4) Renvoyer l’occupation des voisins (EA)
     *   5) Renvoyer les aspects et feux de direction en cours
     */

    envoyerTopologieDepuisSettings();   
    envoyerConfigurationSignauxDepuisSettings();
    envoyerConfigurationServosDepuisSettings();  
    envoyerOccupationDepuisEtatCourant();
    envoyerAspectsDepuisEtatCourant();
    envoyerFeuxDepuisEtatCourant();

    SA_LOG_TRACE("[SatEXSA_Link] Resynchronisation complète envoyée\n");
}

void SatEXSA_Link::onExsaOffline(uint8_t index)
{
    SA_LOG_WARN("[SatEXSA_Link] EXSA %u OFFLINE (timeout PONG)\n", index);

    /*
     * Ici, tu peux décider de la politique en cas de perte EXSA :
     *
     *   - marquer le module comme HS
     *   - bloquer les commandes d’aiguille
     *   - forcer un aspect de sécurité
     *   - afficher un diagnostic
     *
     * Pour l’instant, on se contente de logguer l’événement.
     */
}
