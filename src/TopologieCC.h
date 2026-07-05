/*
 * TopologieCC.h — Gestion Canton 2026
 * ------------------------------------------------------------
 * Déclaration des fonctions de mise à jour de la topologie
 * des CantonController voisins pour le Canton Controller (CC).
 *
 * Le CC calcule, en fonction des aiguilles locales, quels
 * CantonController sont connectés côté horaire (SP1) et côté
 * anti‑horaire (SM1). Ces informations sont ensuite utilisées
 * par l’Extension Canton Controller (EXCC).
 */

#pragma once

#include "Canton.h"
#include "SensEnum.h"

// Met à jour SP1_idx et SM1_idx en fonction des aiguilles locales
void mettreAJourTopologie(Canton *canton);
