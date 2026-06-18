#pragma once

#include <stdint.h>

class Canton;

/*
 * SupervisionEssieux.h — Gestion Canton 2026
 * ------------------------------------------------------------
 * Module de supervision de la cohérence essieux pour le
 * Canton Controller (CC).
 *
 * Rôle du module :
 *   - surveiller la cohérence entre :
 *       • l’occupation logique du canton (Canton::busy())
 *       • le compteur global d’essieux fourni par l’EXCC
 *       • l’état des cantons voisins (amont / aval)
 *       • les reboot EXCC (perte d’état interne)
 *
 * Pourquoi ?
 *   Le compteur d’essieux peut devenir incohérent dans plusieurs cas :
 *       • reboot EXCC
 *       • compteur négatif ou impossible
 *       • topologie incohérente (tout libre mais compteur > 0)
 *
 * Ce module :
 *   - détecte les incohérences
 *   - remet le compteur à zéro dans les cas sûrs
 *   - NE MODIFIE JAMAIS Canton::busy()
 *     (l’occupation logique reste gérée par ConsoCourant)
 *
 * Timer anti‑faux‑positifs :
 *   Permet d’éviter une remise à zéro trop rapide lorsque
 *   le train est mal alimenté ou à cheval sur deux cantons.
 */

class SupervisionEssieux
{
public:
    // Initialisation avec le Canton local
    static void begin(Canton *canton);

    // À appeler régulièrement dans la loop() du Canton Controller
    static void loop();

    // À appeler lorsqu’un reboot EXCC est détecté (PING/PONG)
    static void notifierRebootEXCC();

private:
    static Canton *s_canton;     // Canton supervisé
    static bool s_rebootDetecte; // Flag : reboot EXCC détecté

    // Timer anti‑faux‑positifs
    static uint16_t s_incoherenceTimer;             // compteur interne
    static const uint16_t INCOHERENCE_TIMEOUT = 40; // ~2 secondes

    // Vérifie toutes les règles de cohérence
    static void verifierCoherence();
};
