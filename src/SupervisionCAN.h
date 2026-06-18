/*
 * SupervisionCAN.h — Gestion Canton 2026
 * ------------------------------------------------------------
 * Déclaration des fonctions de supervision CAN pour le
 * Canton Controller (CC).
 *
 * Rôle :
 *   - transmettre l’état ferroviaire local sur le bus CAN
 *   - informer les cantons voisins et l’EXCC :
 *       • occupation locale
 *       • accessibilité SP1 / SM1
 *       • occupation SP1 / SM1
 *   - envoyer la réservation locomotive (trame 0xE3)
 *
 * Voir SupervisionCAN.cpp pour l’implémentation.
 */

#pragma once

#include "Canton.h"

// Envoie les trames 0xE0 (état ferroviaire) et 0xE3 (réservation)
void envoyerEtatCAN(Canton *canton);
