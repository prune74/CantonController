#pragma once

#include <stdint.h>

class Canton;

/*
 * ConsoCourant.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Module de fusion occupation physique (EXCC) + compteur d’essieux.
 *
 * Rôle :
 *   - recevoir l’occupation physique envoyée par l’EXCC (PROTO_04)
 *   - fusionner :
 *        • occupation physique (détection EXCC)
 *        • occupation logique (compteur d’essieux)
 *   - mettre à jour l’état du canton via canton->busy()
 *
 * Ce module ne contient :
 *   - aucune logique de voisinage
 *   - aucune logique de sécurité globale
 *   - aucune interprétation topologique
 *
 * Un CC = un seul ConsoCourant → singleton.
 */

class ConsoCourant
{
public:
    ConsoCourant();
    ~ConsoCourant();

    // -----------------------------------------------------------------------
    // Singleton (un CC = un seul ConsoCourant)
    // -----------------------------------------------------------------------
    static ConsoCourant *s_instance;

    // -----------------------------------------------------------------------
    // setup() — associe ce capteur virtuel au canton local
    // -----------------------------------------------------------------------
    void setup(Canton *canton);

    // -----------------------------------------------------------------------
    // onOccupation() — callback appelé par CC_UartRx (PROTO_04)
    //   index_excc : 0 = côté Horaire, 1 = côté Anti‑Horaire
    //   code       : PROTO_OCC_ACTIVE / PROTO_OCC_LIBRE
    // -----------------------------------------------------------------------
    static void onOccupation(uint8_t index_excc, uint8_t code);

    // -----------------------------------------------------------------------
    // updateEtat() — fusion occupation physique + compteur d’essieux
    // -----------------------------------------------------------------------
    void updateEtat(bool occupePhysique);

private:
    Canton *m_canton = nullptr;
};
