/*
   Signal.h - Gestion enrichie des signaux
*/

#pragma once
#include <stdint.h>

// Types de signaux physiques (compatibles Exploration)
enum SignalProfil : uint8_t
{
    SIG_SIMPLE = 0, // 3 feux : O/R/V
    SIG_CARRE = 1,  // Carré + œilleton
    SIG_RAL = 2,    // Ralentissement (jusqu'à 9 feux)
    SIG_RAPPEL = 3, // Rappel ralentissement (jusqu'à 9 feux)

    // --- Nouveaux types (préparés pour l’évolution) ---
    SIG_MANOEUVRE = 4, // Carré violet + blanc
    SIG_BAL = 5,       // BAL (VL clignotant possible)
    SIG_ENTREE = 6,    // Entrée de gare (souvent 3x3)
    SIG_SORTIE = 7     // Sortie de gare (souvent 3x3)
};

class Signal
{
public:
    Signal();
    ~Signal();

    void setup();

    void type(uint8_t t);
    uint8_t type() const;

    void position(uint8_t p);
    uint8_t position() const;

    uint8_t length() const;

    // --- Capacités physiques du signal ---
    bool hasCarre() const { return m_hasCarre; }
    bool hasOeilleton() const { return m_hasOeilleton; }
    bool hasRal() const { return m_hasRal; }
    bool hasRappel() const { return m_hasRappel; }
    bool hasManoeuvre() const { return m_hasManoeuvre; }
    bool hasCarreViolet() const { return m_hasCarreViolet; }
    bool hasVLclignotant() const { return m_hasVLclignotant; }

private:
    uint8_t m_type;
    uint8_t m_position;
    uint8_t m_length;

    // Capacités internes
    bool m_hasCarre;
    bool m_hasOeilleton;
    bool m_hasRal;
    bool m_hasRappel;
    bool m_hasManoeuvre;
    bool m_hasCarreViolet;
    bool m_hasVLclignotant;
};
