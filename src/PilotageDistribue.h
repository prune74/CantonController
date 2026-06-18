/*
 * PilotageDistribue.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Pilotage distribué des locomotives selon l’aspect reçu du canton voisin.
 *
 * Ce module :
 *   - lit le sens de marche de la locomotive
 *   - sélectionne le voisin pertinent (SP1 ou SM1)
 *   - récupère l’aspect SNCF reçu via CantonPeriph
 *   - applique la vitesse correspondante à la locomotive
 *
 * Aucun calcul d’aspect n’est effectué ici : la logique métier est
 * entièrement gérée par SupervisionCanton.
 */

#pragma once

#include "Canton.h"

// Exécute le pilotage distribué pour le canton donné
void executerPilotageDistribue(Canton *canton);
