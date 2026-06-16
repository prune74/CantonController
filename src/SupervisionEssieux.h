#pragma once

#include <stdint.h>

class Canton;

/*
 * ============================================================
 *  SupervisionEssieux — SA 2026
 * ------------------------------------------------------------
 *  Rôle :
 *    Ce module surveille la cohérence ferroviaire entre :
 *
 *      - l’occupation logique du canton (Canton::busy())
 *      - le compteur global d’essieux (fourni par EXCC)
 *      - l’état des cantons amont et aval
 *      - les reboot EXCC (via notifierRebootEXCC)
 *
 *    Pourquoi ?
 *      Le compteur d’essieux peut devenir invalide dans plusieurs cas :
 *        - reboot EXCC (perte de l’état interne)
 *        - compteur impossible (ex : < 0)
 *        - incohérence topologique (tout libre mais compteur > 0)
 *
 *    Ce module :
 *      - détecte ces incohérences
 *      - remet le compteur à zéro dans les cas sûrs
 *      - NE MODIFIE JAMAIS Canton::busy()
 *        (l’occupation logique reste gérée par ConsoCourant)
 *
 *    🔥 Timer anti‑faux‑positifs :
 *      Pour éviter de réinitialiser le compteur trop vite
 *      lorsqu’un train est mal alimenté ou à cheval sur deux cantons.
 * ============================================================
 */

class SupervisionEssieux
{
public:
    // Initialisation avec le Canton local
    static void begin(Canton *canton);

    // À appeler régulièrement dans la loop() du SA
    static void loop();

    // À appeler lorsqu’un reboot EXCC est détecté (PING/PONG)
    static void notifierRebootEXCC();

private:
    static Canton *s_canton;     // Canton supervisé
    static bool s_rebootDetecte; // Flag : reboot EXCC détecté

    // 🔥 Timer anti‑faux‑positifs
    static uint16_t s_incoherenceTimer;             // compteur interne
    static const uint16_t INCOHERENCE_TIMEOUT = 40; // ~2 secondes

    // Vérifie toutes les règles de cohérence
    static void verifierCoherence();
};
