/*
 * ConsoCourant.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Module de fusion occupation physique (EXCC) + compteur d’essieux.
 *
 * Trame reçue depuis EXCC :
 *   [SYNC][0x04][index_excc][code]
 *
 * index_excc :
 *   0 = côté Horaire
 *   1 = côté Anti‑Horaire
 *
 * Rappel :
 *   - un CC = un canton
 *   - cette information concerne TOUJOURS le canton local
 *
 * Rôle :
 *   - recevoir l’occupation physique (PROTO_OCC_ACTIVE / LIBRE)
 *   - fusionner avec le compteur d’essieux
 *   - mettre à jour l’état logique du canton (busy)
 *
 * Ce module est volontairement simple :
 *   - aucune logique de voisinage
 *   - aucune sécurité globale
 *   - aucune interprétation topologique
 */

#include "ConsoCourant.h"
#include "Exploration_Protocol.h"
#include "Canton.h"
#include "debug_cc.h"

// ---------------------------------------------------------------------------
// Singleton (un CC = un seul ConsoCourant)
// ---------------------------------------------------------------------------
ConsoCourant *ConsoCourant::s_instance = nullptr;

// ---------------------------------------------------------------------------
// Constructeur / Destructeur
// ---------------------------------------------------------------------------
ConsoCourant::ConsoCourant() {}
ConsoCourant::~ConsoCourant() {}

// ---------------------------------------------------------------------------
// setup()
// ---------------------------------------------------------------------------
void ConsoCourant::setup(Canton *canton)
{
    m_canton = canton;
    s_instance = this;
}

// ---------------------------------------------------------------------------
// updateEtat() — fusion occupation physique + essieux
// ---------------------------------------------------------------------------
void ConsoCourant::updateEtat(bool occupePhysique)
{
    if (!m_canton)
    {
        CC_LOG_ERROR("[ConsoCourant][CC] m_canton nul\n");
        return;
    }

    int compteur = Canton::s_instance->compteurEssieux();
    bool occupeLogique = occupePhysique || (compteur > 0);

    m_canton->busy(occupeLogique);

    CC_LOG_INFO(
        "[ConsoCourant][CC] Canton = %s (phys=%d, essieux=%d)\n",
        occupeLogique ? "OCCUPE" : "LIBRE",
        occupePhysique ? 1 : 0,
        compteur);
}

// ---------------------------------------------------------------------------
// onOccupation() — callback appelé par CC_UartRx
// ---------------------------------------------------------------------------
void ConsoCourant::onOccupation(uint8_t index_excc, uint8_t code)
{
    bool occPhys = false;

    switch (code)
    {
    case PROTO_OCC_ACTIVE:
        occPhys = true;
        break;

    case PROTO_OCC_LIBRE:
        occPhys = false;
        break;

    default:
        CC_LOG_WARN("[ConsoCourant][CC] Code occupation inconnu : %02X\n", code);
        return;
    }

    if (!s_instance)
    {
        CC_LOG_ERROR("[ConsoCourant][CC] s_instance nul dans onOccupation()\n");
        return;
    }

    s_instance->updateEtat(occPhys);
}
