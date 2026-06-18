/*
 * SupervisionAiguilles.h — Gestion Canton 2026
 * ------------------------------------------------------------
 * Supervision des aiguilles pour le Canton Controller (CC).
 *
 * Rôle :
 *   - recevoir les positions d’aiguilles envoyées par l’EXCC
 *   - mettre à jour l’état logique des aiguilles dans le CC
 *   - mettre à jour le masque global des aiguilles
 *
 * Ce module reflète l’état réel remonté par l’Extension
 * Canton Controller (EXCC). Il ne pilote pas les servos.
 */

#pragma once

#include <stdint.h>

class Canton;

class SupervisionAiguilles
{
public:
    // Initialisation avec le Canton Controller
    static void begin(Canton *canton);

    // Notification EXCC : mise à jour d’une aiguille
    static void onPosition(uint8_t index_excc,
                           uint8_t idAig,
                           uint8_t etat,
                           uint8_t masque);

private:
    static Canton *s_canton; // Canton supervisé
};
