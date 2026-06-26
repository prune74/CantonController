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
 *   - envoyer la réservation locomotive (trame CMD_EXPLOITATION_RESERVATION_LOCO)
 *
 * Voir SupervisionCAN.cpp pour l’implémentation.
 */

#pragma once

#include "Canton.h"

// Envoie les trames CMD_EXPLOITATION_UPDATE_VOISINS (état ferroviaire) et CMD_EXPLOITATION_RESERVATION_LOCO (réservation)
void envoyerEtatCAN(Canton *canton);

// Met à jour la LED topologie (OK / erreur)
void updateTopoLed();
