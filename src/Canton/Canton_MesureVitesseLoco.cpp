#include "Canton.h"
#include "Settings.h"
#include "CC_CAN.h"
#include "debug_cc.h"

// ---------------------------------------------------------------------------
// Variables internes au module (pas dans Canton)
// ---------------------------------------------------------------------------
static bool mesureActive = false;
static uint32_t t0 = 0;
static uint16_t locoMesure = 0;

// États précédents des ponctuels (pour détecter les transitions)
static bool prevH = false;
static bool prevAH = false;

// ---------------------------------------------------------------------------
// Armement de la mesure
// ---------------------------------------------------------------------------
void Canton::armer(Canton *canton, uint16_t locoID)
{
    mesureActive = true;
    locoMesure = locoID;

    CC_LOG_INFO("[Mesure][Canton %u] Mesure armée pour loco %u\n",
                canton->ID(), locoID);
}

// ---------------------------------------------------------------------------
// Détection des transitions ponctuelH / ponctuelAH
// ---------------------------------------------------------------------------
void Canton::update(Canton *canton)
{
    if (!mesureActive || !canton)
        return;

    bool h = canton->ponctuelH();
    bool ah = canton->ponctuelAH();

    // --- Détection transition H ---
    if (h && !prevH)
        onEntree(canton);

    if (!h && prevH)
        onSortie(canton);

    // --- Détection transition AH ---
    if (ah && !prevAH)
        onEntree(canton);

    if (!ah && prevAH)
        onSortie(canton);

    prevH = h;
    prevAH = ah;
}

// ---------------------------------------------------------------------------
// Ponctuel d’entrée → capture t0
// ---------------------------------------------------------------------------
void Canton::onEntree(Canton *canton)
{
    t0 = millis();

    CC_LOG_INFO("[Mesure][Canton %u] t0 = %lu ms (entrée)\n",
                canton->ID(), t0);
}

// ---------------------------------------------------------------------------
// Ponctuel de sortie → capture t1 + calcul + stockage interne
// ---------------------------------------------------------------------------
void Canton::onSortie(Canton *canton)
{
    uint32_t t1 = millis();
    uint32_t dt = t1 - t0;

    uint16_t L = Settings::longueurCantonMM();
    float v = (float)L / (float)dt;

    CC_LOG_INFO("[Mesure][Canton %u] vitesse mesurée = %.3f (loco %u)\n",
                canton->ID(), v, locoMesure);

    m_vitesseMesuree = v;
    m_locoMesuree = locoMesure;
    m_mesureVitesseDisponible = true;

    uint16_t v1000 = (uint16_t)(v * 1000.0f);

    CC_LOG_INFO("[CAN][Mesure][CC] Envoi vitesse %.3f pour loco %u\n",
                v, locoMesure);

    CC_CAN::sendMsg(
        0,
        (uint8_t)Cmd_CC_to_ERM::MESURE_VITESSE,
        0,
        canton->ID(),
        locoMesure >> 8,
        locoMesure & 0xFF,
        v1000 >> 8,
        v1000 & 0xFF
    );

    mesureActive = false;
}
