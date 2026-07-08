/*
 * Signal.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Représentation d’un signal SNCF physique pour le Canton Controller (CC).
 *
 * Ce module définit les capacités physiques d’un signal selon son type :
 *   - 2 feux  : manœuvre
 *   - 3 feux  : BAL
 *   - 5 feux  : carré
 *   - 7 feux  : ralentissement
 *   - 9 feux  : rappel / entrée / sortie
 *
 * L’allumage réel des feux est assuré par EXCC_Signaux.cpp.
 */

#include "Signal.h"
#include <Protocol.h>
#include "debug_cc.h"

// ---------------------------------------------------------------------------
// Constructeur
// Initialise toutes les capacités à false.
// Le type réel sera défini plus tard via type().
// ---------------------------------------------------------------------------
Signal::Signal() // 🟢
    : m_type(0),
      m_position(0),
      m_hasCarre(false),
      m_hasOeilleton(false),
      m_hasRal(false),
      m_hasRappel(false),
      m_hasManoeuvre(false),
      m_hasCarreViolet(false),
      m_hasVLclignotant(false)
{
    CC_LOG_TRACE("[Signal][CC] Constructeur : type=INDEFINI position=%u\n",
                 m_position);
}

Signal::~Signal() {}

// ---------------------------------------------------------------------------
// setup()
// Configure les capacités du signal en fonction de son type physique.
// ---------------------------------------------------------------------------
void Signal::setup() // 🟢
{
    // Réinitialisation des capacités internes
    m_hasCarre = false;
    m_hasOeilleton = false;
    m_hasRal = false;
    m_hasRappel = false;
    m_hasManoeuvre = false;
    m_hasCarreViolet = false;
    m_hasVLclignotant = false;

    CC_LOG_TRACE("[Signal][CC] setup() : configuration du type %u\n", m_type);

    switch (m_type)
    {
    case TYPE_A: // 1 — 3 feux : Rouge / Jaune / Vert (BAL)
        // m_length = 3;
        m_hasVLclignotant = true; // capacité BAL : vert clignotant
        CC_LOG_INFO("[Signal][CC] BAL → 3 feux (R/J/V + VL clignotant)\n");
        break;

    case TYPE_C: // 2 — 5 feux : Carré + Œilleton
        // m_length = 5;
        m_hasCarre = true;     // 2 rouges
        m_hasOeilleton = true; // œilleton blanc
        CC_LOG_INFO("[Signal][CC] CARRE → 5 feux (Carré + Œilleton)\n");
        break;

    case TYPE_E: // 3 — 7 feux : Ralentissement 30/60
        // m_length = 7;
        m_hasRal = true; // jaune fixe + jaune clignotant + vert
        CC_LOG_INFO("[Signal][CC] RAL → 7 feux (Ralentissement 30/60)\n");
        break;

    case TYPE_G: // 4 — 9 feux : Rappel de ralentissement
        // m_length = 9;
        m_hasRappel = true; // confirmation du ralentissement
        CC_LOG_INFO("[Signal][CC] RAPPEL → 9 feux (Rappel 30/60)\n");
        break;

    case TYPE_M: // 5 — 2 feux : Blanc + Violet
        // m_length = 2;
        m_hasManoeuvre = true;
        m_hasCarreViolet = true; // violet = carré de manœuvre
        CC_LOG_INFO("[Signal][CC] MANOEUVRE → 2 feux (Blanc + Violet)\n");
        break;

    case SIG_ABSENT: // 0 — aucun signal présent physiquement
        // m_length = 0;
        CC_LOG_INFO("[Signal][CC] ABSENT → aucun signal\n");
        break;

    default:
        // m_length = 0;
        CC_LOG_WARN("[Signal][CC] Type inconnu (%u) → INDEFINI\n", m_type);
        break;
    }

    CC_LOG_TRACE("[Signal][CC] setup() terminé : length=%u\n", m_length);
}

// ---------------------------------------------------------------------------
// Setters / Getters
// ---------------------------------------------------------------------------
void Signal::type(uint8_t t) // 🟢
{
    m_type = t;
    CC_LOG_TRACE("[Signal][CC] type() = %u\n", t);
}

uint8_t Signal::type() const { return m_type; } // 🟢

void Signal::position(uint8_t p) // 🟢
{
    m_position = p;
    CC_LOG_TRACE("[Signal][CC] position() = %u\n", p);
}

uint8_t Signal::position() const { return m_position; } // 🟢

// uint8_t Signal::length() const { return m_length; } // 🔴
