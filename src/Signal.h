/*
 * Signal.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Définition d’un signal SNCF physique ou logique pour le
 * Canton Controller (CC).
 *
 * Un signal possède :
 *   - un type (simple, carré, ralentissement, rappel, BAL, manœuvre…)
 *   - une position dans le canton (0 = horaire, 1 = anti‑horaire)
 *   - un nombre de feux (length)
 *   - des capacités physiques (carré, œilleton, rappel, etc.)
 *
 * Ce module décrit uniquement la structure et les capacités du signal.
 * L’allumage réel des feux est assuré par l’Extension Canton Controller
 * (EXCC), dans EXCC_Signaux.cpp.
 */

#pragma once
#include <stdint.h>

// ---------------------------------------------------------------------------
// Types de signaux physiques SNCF
// ---------------------------------------------------------------------------
enum SignalProfil : uint8_t
{
    SIG_SIMPLE    = 0,   // Rouge / Jaune / Vert
    SIG_CARRE     = 1,   // Carré + Œilleton
    SIG_RAL       = 2,   // Ralentissement (3×3 feux)
    SIG_RAPPEL    = 3,   // Rappel de ralentissement (3×3 feux)

    // Types étendus
    SIG_MANOEUVRE = 4,   // Blanc + Violet
    SIG_BAL       = 5,   // BAL (VL clignotant)
    SIG_ENTREE    = 6,   // Entrée de gare (souvent 3×3)
    SIG_SORTIE    = 7,   // Sortie de gare (souvent 3×3)

    SIG_ABSENT    = 255  // Aucun signal
};

// ---------------------------------------------------------------------------
// Classe Signal
// ---------------------------------------------------------------------------
class Signal
{
public:
    Signal();
    ~Signal();

    // Configure les capacités selon le type
    void setup();

    // Type du signal
    void type(uint8_t t);
    uint8_t type() const;

    // Position dans le canton (0 = H, 1 = AH)
    void position(uint8_t p);
    uint8_t position() const;

    // Nombre de feux
    uint8_t length() const;

    // Capacités physiques
    bool hasCarre() const        { return m_hasCarre; }
    bool hasOeilleton() const    { return m_hasOeilleton; }
    bool hasRal() const          { return m_hasRal; }
    bool hasRappel() const       { return m_hasRappel; }
    bool hasManoeuvre() const    { return m_hasManoeuvre; }
    bool hasCarreViolet() const  { return m_hasCarreViolet; }
    bool hasVLclignotant() const { return m_hasVLclignotant; }

private:
    uint8_t m_type;      // Profil du signal
    uint8_t m_position;  // 0 = horaire, 1 = anti‑horaire
    uint8_t m_length;    // Nombre de feux

    // Capacités internes
    bool m_hasCarre;
    bool m_hasOeilleton;
    bool m_hasRal;
    bool m_hasRappel;
    bool m_hasManoeuvre;
    bool m_hasCarreViolet;
    bool m_hasVLclignotant;
};
