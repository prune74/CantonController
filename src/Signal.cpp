/*
 * Signal.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Représentation d’un signal SNCF physique ou logique pour le
 * Canton Controller (CC).
 *
 * Un signal possède :
 *   - un type (simple, carré, ralentissement, rappel, BAL, manœuvre…)
 *   - une position dans le tableau des signaux du canton
 *   - un nombre de feux (m_length)
 *   - des capacités (présence d’un carré, œilleton, rappel, etc.)
 *
 * Ce module définit les capacités d’un signal selon son type.
 * Il ne gère PAS l’allumage des feux : cette partie est assurée par
 * l’Extension Canton Controller (EXCC) dans EXCC_Signaux.cpp.
 */

#include "Signal.h"
#include "debug_cc.h"

// ---------------------------------------------------------------------------
// Constructeur
// Initialise toutes les capacités à false.
// Le type réel sera défini plus tard via type().
// ---------------------------------------------------------------------------
Signal::Signal()
    : m_type(0),
      m_position(0),
      m_length(0),
      m_hasCarre(false),
      m_hasOeilleton(false),
      m_hasRal(false),
      m_hasRappel(false),
      m_hasManoeuvre(false),
      m_hasCarreViolet(false),
      m_hasVLclignotant(false)
{
    CC_LOG_TRACE("[Signal][CC] Constructeur : type=%u position=%u\n",
                 m_type, m_position);
}

Signal::~Signal() {}

// ---------------------------------------------------------------------------
// setup()
// Configure les capacités du signal en fonction de son type.
// ---------------------------------------------------------------------------
void Signal::setup()
{
    // Réinitialisation des capacités
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
    case SIG_SIMPLE: // 0
        m_length = 3;
        CC_LOG_INFO("[Signal][CC] SIMPLE → 3 feux\n");
        break;

    case SIG_CARRE: // 1
        m_length = 5;
        m_hasCarre = true;
        m_hasOeilleton = true;
        CC_LOG_INFO("[Signal][CC] CARRE → 5 feux (Carré + Œilleton)\n");
        break;

    case SIG_RAL: // 2
        m_length = 9;
        m_hasRal = true;
        CC_LOG_INFO("[Signal][CC] RAL → 9 feux (RAL 30/60)\n");
        break;

    case SIG_RAPPEL: // 3
        m_length = 9;
        m_hasRappel = true;
        CC_LOG_INFO("[Signal][CC] RAPPEL → 9 feux (Rappel 30/60)\n");
        break;

    case SIG_MANOEUVRE: // 4
        m_length = 2;
        m_hasManoeuvre = true;
        m_hasCarreViolet = true;
        CC_LOG_INFO("[Signal][CC] MANOEUVRE → Blanc + Violet\n");
        break;

    case SIG_BAL: // 5
        m_length = 3;
        m_hasVLclignotant = true;
        CC_LOG_INFO("[Signal][CC] BAL → Rouge / Jaune / Vert clignotant\n");
        break;

    case SIG_ENTREE: // 6
        m_length = 9;
        m_hasCarre = true;
        m_hasRal = true;
        CC_LOG_INFO("[Signal][CC] ENTREE → Carré + RAL\n");
        break;

    case SIG_SORTIE: // 7
        m_length = 9;
        m_hasRappel = true;
        CC_LOG_INFO("[Signal][CC] SORTIE → Rappel\n");
        break;

    default:
        m_length = 3;
        CC_LOG_WARN("[Signal][CC] Type inconnu (%u) → fallback SIMPLE\n", m_type);
        break;
    }

    CC_LOG_TRACE("[Signal][CC] setup() terminé : length=%u\n", m_length);
}

// ---------------------------------------------------------------------------
// Setters / Getters
// ---------------------------------------------------------------------------
void Signal::type(uint8_t t)
{
    m_type = t;
    CC_LOG_TRACE("[Signal][CC] type() = %u\n", t);
}

uint8_t Signal::type() const { return m_type; }

void Signal::position(uint8_t p)
{
    m_position = p;
    CC_LOG_TRACE("[Signal][CC] position() = %u\n", p);
}

uint8_t Signal::position() const { return m_position; }

uint8_t Signal::length() const { return m_length; }
