/*
 * Occupation.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Nouveau rôle :
 *   - Recevoir l’occupation finale depuis l’EXCC (OCCUPATION)
 *   - Mettre à jour l’état logique du canton (busy)
 */

#include "Occupation.h"
#include <Protocol.h>
#include "Canton.h"
#include "debug_cc.h"

// ---------------------------------------------------------------------------
// Singleton
// ---------------------------------------------------------------------------
Occupation *Occupation::s_instance = nullptr;

// ---------------------------------------------------------------------------
// Constructeur / Destructeur
// ---------------------------------------------------------------------------
Occupation::Occupation() {}
Occupation::~Occupation() {}

// ---------------------------------------------------------------------------
// setup()
// ---------------------------------------------------------------------------
void Occupation::setup(Canton *canton)
{
    m_canton = canton;
    s_instance = this;

    CC_LOG_INFO("[Occupation][CC] Module initialisé pour Canton %d\n", canton->ID());
}

// ---------------------------------------------------------------------------
// updateEtat() — applique l’occupation envoyée par l’EXCC
// ---------------------------------------------------------------------------
void Occupation::updateEtat(bool occupe)
{
    if (!m_canton)
    {
        CC_LOG_ERROR("[Occupation][CC] m_canton nul\n");
        return;
    }

    m_canton->busy(occupe);

    CC_LOG_INFO("[Occupation][CC] Canton = %s\n",
                occupe ? "OCCUPE" : "LIBRE");
}

// ---------------------------------------------------------------------------
// onOccupation()
// ---------------------------------------------------------------------------
void Occupation::onOccupation(uint8_t code)
{
    bool occupe = false;

    switch (code)
    {
    case static_cast<uint8_t>(ExccCode::OCC_ACTIVE):
        occupe = true;
        break;

    case static_cast<uint8_t>(ExccCode::OCC_LIBRE):
        occupe = false;
        break;

    default:
        CC_LOG_WARN("[Occupation][CC] Code occupation inconnu : %02X\n", code);
        return;
    }

    if (!s_instance)
    {
        CC_LOG_ERROR("[Occupation][CC] s_instance nul dans onOccupation()\n");
        return;
    }

    s_instance->updateEtat(occupe);
}
