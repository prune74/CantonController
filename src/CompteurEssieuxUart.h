#pragma once
#include <stdint.h>
#include <Arduino.h>
#include "Config.h"

/*
 * ============================================================
 *  CompteurEssieuxUart — SA 2026
 * ------------------------------------------------------------
 *  Rôle :
 *    - Lire les trames DELTA AXE envoyées par EXSA
 *    - Identifier si elles viennent du côté H ou AH
 *    - Maintenir un compteur global d’essieux
 *    - Fournir au SA une API simple :
 *         compteurH()
 *         compteurAH()
 *         compteurGlobal()
 *
 *  Pourquoi dans le SA ?
 *    - Chaque EXSA ne voit qu’un seul côté du canton
 *    - Seul le SA peut fusionner H + AH
 *    - Seul le SA peut reconstruire l’occupation logique
 * ============================================================
 */

class CompteurEssieuxUart {
public:

    // Initialisation (optionnelle)
    static void begin();

    // Lecture UART (à appeler dans loop())
    static void loop();

    // Accès aux compteurs
    static int compteurH();
    static int compteurAH();
    static int compteurGlobal();

    // Remise à zéro (utile après reboot EXSA)
    static void reset();

private:

    // Mise à jour interne d’un compteur
    static void traiterDelta(int delta, bool coteHoraire);
};
