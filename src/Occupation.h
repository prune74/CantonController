#pragma once

#include <stdint.h>

class Canton;

/*
 * Occupation.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Nouveau rôle :
 *   - Recevoir l’occupation finale envoyée par l’EXCC (PROTO_04_OCCUPATION)
 *   - Mettre à jour l’état logique du canton via canton->busy()
 *
 * Ce module NE FAIT PLUS :
 *   - aucune fusion avec un compteur d’essieux
 *   - aucune logique d’occupation physique
 *   - aucune logique de voisinage
 *   - aucune interprétation topologique
 *
 * L’EXCC est désormais l’unique source de vérité pour l’occupation.
 */

class Occupation
{
public:
    Occupation();
    ~Occupation();

    // Singleton (un CC = un seul Occupation)
    static Occupation *s_instance;

    // Associe ce module au canton local
    void setup(Canton *canton);

    // Callback appelé par CC_UartRx (PROTO_04)
    static void onOccupation(uint8_t code);

    // Applique l’occupation envoyée par l’EXCC
    void updateEtat(bool occupe);

private:
    Canton *m_canton = nullptr;
};
