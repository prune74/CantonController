#pragma once
#include <Arduino.h>
#include "Config.h"

/*
 * Aig.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Représente une AIGUILLE LOGIQUE du réseau.
 *
 * Rôle :
 *   - Le CC (Canton Controller) ne pilote plus physiquement les servos.
 *   - L’EXCC (côté H ou AH) pilote les servos via PCA9685.
 *   - Cette classe stocke uniquement :
 *        • positions droite / déviée (µs)
 *        • position logique courante
 *        • index EXCC côté H (SP1)
 *        • index EXCC côté AH (SM1)
 *        • vitesse logique (Exploration 2026)
 *
 * Aucun PWM local n’est utilisé dans l’architecture 2026.
 * Le CC transmet les paramètres à l’EXCC via RS485 (F0/F1/F2).
 */

class Aig
{
protected:
    // Identifiant logique de l’aiguille (0..N)
    uint8_t m_id;

    // Positions logiques droite / déviée (µs)
    uint16_t m_posDroit;
    uint16_t m_posDevie;

    // Limites mécaniques (validation des valeurs)
    uint16_t m_minPos;
    uint16_t m_maxPos;

    // État logique actuel (true = droite)
    bool m_estDroit;

    // Position logique courante (EXCC effectuera le mouvement réel)
    uint16_t m_curPos;

    // Index EXCC côté H (SP1)
    uint8_t m_cantonPdroitIdx;

    // Index EXCC côté AH (SM1)
    uint8_t m_cantonPdevieIdx;

    // Vitesse logique (Exploration 2026)
    uint16_t m_speed = 0;

public:
    Aig();
    ~Aig();

    // Identifiant logique
    void ID(uint8_t);
    uint8_t ID() const;

    // État logique (droite / déviée)
    void estDroit(bool);
    bool estDroit() const;

    // Positions logiques droite / déviée
    void posDroit(uint16_t);
    void posDevie(uint16_t);
    uint16_t posDroit() const;
    uint16_t posDevie() const;

    // Position logique courante
    void curPos(uint16_t);
    uint16_t curPos() const;

    // Index EXCC côté H (SP1)
    void cantonPdroitIdx(uint8_t);
    uint8_t cantonPdroitIdx() const;

    // Index EXCC côté AH (SM1)
    void cantonPdevieIdx(uint8_t);
    uint8_t cantonPdevieIdx() const;

    // Vitesse logique (transmise à EXCC)
    void speed(uint16_t v);
    uint16_t speed() const;
};

/* ---------------------------------------------------------------------------
 * Fin de Aig.h
 * ---------------------------------------------------------------------------
 */
