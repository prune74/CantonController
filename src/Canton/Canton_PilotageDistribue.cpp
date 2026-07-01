/*
 * Canton_PilotageDistribue.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Module dédié au pilotage distribué côté Canton.
 *
 * Rôle :
 *   - encapsuler l’appel à executerPilotageDistribue()
 *   - fournir une API propre : canton->pilotageDistribue()
 *   - éviter les appels directs dans les modules externes
 *   - préparer l’intégration avec RalentissementInterneCanton
 *
 * IMPORTANT :
 *   - aucune logique métier ici
 *   - ce module délègue entièrement à executerPilotageDistribue()
 */

#include "Canton.h"
#include "PilotageDistribue.h"
#include "debug_cc.h"

// ---------------------------------------------------------------------------
// API publique : pilotage distribué
// ---------------------------------------------------------------------------
void Canton::pilotageDistribue()
{
    CC_LOG_TRACE("[Canton][PilotageDistribue] Appel pilotage distribué\n");

    executerPilotageDistribue(this);
}
