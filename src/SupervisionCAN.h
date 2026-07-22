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
 *   - envoyer la réservation locomotive (trame RESERVATION_LOCO)
 *
 * Voir SupervisionCAN.cpp pour l’implémentation.
 */

#pragma once
#include "Canton.h"

namespace SupervisionCAN
{
    void envoyerEtatCAN(Canton *canton);
    void updateTopoLed();

    // Ajout de l'index de wagon
    void envoyerRechercheWagon(Canton *canton, uint16_t destId, uint16_t locoId, uint8_t index);
}
