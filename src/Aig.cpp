/*
 * Aig.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Implémentation d’une AIGUILLE LOGIQUE.
 *
 * Le CC (Canton Controller) ne pilote aucun servo :
 *   - l’EXCC (côté H ou AH) effectue le mouvement réel via PCA9685.
 *   - cette classe stocke uniquement l’état LOGIQUE :
 *        • positions droite / déviée (µs)
 *        • position logique courante
 *        • index EXCC côté H / AH
 *        • vitesse logique (option Exploration 2026)
 *
 * Le CC transmet ces valeurs à l’EXCC via RS485 (F0/F1/F2).
 */

#include "Aig.h"

// ---------------------------------------------------------------------------
// Constructeur : initialise toutes les valeurs LOGIQUES
// ---------------------------------------------------------------------------
Aig::Aig()
    : m_id(0),
      m_posDroit(1500),
      m_posDevie(1500),
      m_minPos(800),
      m_maxPos(2400),
      m_estDroit(true),
      m_curPos(1500),
      m_cantonPdroitIdx(0),
      m_cantonPdevieIdx(0),
      m_speed(0)
{
}

// ---------------------------------------------------------------------------
// Destructeur
// ---------------------------------------------------------------------------
Aig::~Aig() {}

// ---------------------------------------------------------------------------
// Identifiant logique
// ---------------------------------------------------------------------------
void Aig::ID(uint8_t id) { m_id = id; } // 🟢
uint8_t Aig::ID() const { return m_id; } // 🟢

// ---------------------------------------------------------------------------
// État logique (droite / déviée)
// ---------------------------------------------------------------------------
void Aig::estDroit(bool pos) { m_estDroit = pos; } // 🟢
bool Aig::estDroit() const { return m_estDroit; } // 🟢

// ---------------------------------------------------------------------------
// Positions logiques droite / déviée
// ---------------------------------------------------------------------------
void Aig::posDroit(uint16_t pos) // 🟢
{
    if (pos > m_minPos && pos < m_maxPos)
        m_posDroit = pos;
}

void Aig::posDevie(uint16_t pos) // 🟢
{
    if (pos > m_minPos && pos < m_maxPos)
        m_posDevie = pos;
}

uint16_t Aig::posDroit() const { return m_posDroit; } // 🟢
uint16_t Aig::posDevie() const { return m_posDevie; } // 🟢

// ---------------------------------------------------------------------------
// Position logique courante (EXCC effectuera le mouvement réel)
// ---------------------------------------------------------------------------
void Aig::curPos(uint16_t pos) // 🔴
{
    if (pos > m_minPos && pos < m_maxPos)
        m_curPos = pos;
}

uint16_t Aig::curPos() const { return m_curPos; } // 🔴

// ---------------------------------------------------------------------------
// Index EXCC côté H (SP1)
// ---------------------------------------------------------------------------
void Aig::cantonPdroitIdx(uint8_t idx) { m_cantonPdroitIdx = idx; } // 🟢
uint8_t Aig::cantonPdroitIdx() const { return m_cantonPdroitIdx; } // 🟢

// ---------------------------------------------------------------------------
// Index EXCC côté AH (SM1)
// ---------------------------------------------------------------------------
void Aig::cantonPdevieIdx(uint8_t idx) { m_cantonPdevieIdx = idx; } // 🟢
uint8_t Aig::cantonPdevieIdx() const { return m_cantonPdevieIdx; } // 🟢

// ---------------------------------------------------------------------------
// Vitesse logique (CC → EXCC)
// ---------------------------------------------------------------------------
void Aig::speed(uint16_t v) // 🟡
{
    m_speed = v;
}

uint16_t Aig::speed() const // 🟡
{
    return m_speed;
}

/* ---------------------------------------------------------------------------
 * Fin de Aig.cpp
 * ---------------------------------------------------------------------------
 */
