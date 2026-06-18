/*
 * SupervisionAiguilles.h — Gestion Canton 2026
 * ------------------------------------------------------------
 * Supervision des aiguilles pour le Canton Controller (CC).
 *
 * Rôle :
 *   - recevoir les positions d’aiguilles envoyées par l’EXCC
 *   - mettre à jour l’état logique des aiguilles dans le CC
 *
 * IMPORTANT 2026 :
 *   - aucun masque d’aiguilles n’est maintenu côté Canton
 *   - ce module ne pilote pas les servos
 *   - il reflète uniquement l’état réel remonté par l’EXCC
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
    static void onPosition(uint8_t idAig,
                           uint8_t pos,
                           uint8_t etat);

private:
    static Canton *s_canton; // Canton supervisé
};
