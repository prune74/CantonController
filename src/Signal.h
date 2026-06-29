/*
 * Signal.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Définition d’un signal SNCF physique pour le Canton Controller (CC).
 *
 * Un signal possède :
 *   - un type physique (2, 3, 5, 7 ou 9 feux)
 *   - une position dans le canton (0 = horaire, 1 = anti‑horaire)
 *   - un nombre de feux (length)
 *   - des capacités physiques (présence d’un carré, œilleton, rappel, etc.)
 *
 * Ce module décrit uniquement la structure et les capacités du signal.
 * L’allumage réel des feux est assuré par l’Extension Canton Controller
 * (EXCC), dans EXCC_Signaux.cpp.
 */

#pragma once
#include <stdint.h>

// ---------------------------------------------------------------------------
// Classe Signal
// ---------------------------------------------------------------------------
class Signal
{
public:
    Signal();
    ~Signal();

    // Configure les capacités selon le type physique
    void setup();

    // Type du signal
    void type(uint8_t t);
    uint8_t type() const;

    // Position dans le canton (0 = H, 1 = AH)
    void position(uint8_t p);
    uint8_t position() const;

    // Nombre de feux physiques
    uint8_t length() const;

    // Capacités physiques
    bool hasCarre() const { return m_hasCarre; }
    bool hasOeilleton() const { return m_hasOeilleton; }
    bool hasRal() const { return m_hasRal; }
    bool hasRappel() const { return m_hasRappel; }
    bool hasManoeuvre() const { return m_hasManoeuvre; }
    bool hasCarreViolet() const { return m_hasCarreViolet; }
    bool hasVLclignotant() const { return m_hasVLclignotant; }

private:
    uint8_t m_type;     // Profil du signal (nombre de feux)
    uint8_t m_position; // 0 = horaire, 1 = anti‑horaire
    //uint8_t m_length;   // Nombre de feux physiques

    // Capacités internes
    bool m_hasCarre;
    bool m_hasOeilleton;
    bool m_hasRal;
    bool m_hasRappel;
    bool m_hasManoeuvre;
    bool m_hasCarreViolet;
    bool m_hasVLclignotant;
};
