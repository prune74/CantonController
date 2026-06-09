/*
 * Signal.cpp — Gestion enrichie des signaux SNCF (Version Exploration 2026)
 * -----------------------------------------------------------------------
 * Ce module représente un signal ferroviaire physique ou logique.
 *
 * Chaque signal possède :
 *   - un type (simple, carré, ralentissement, rappel, BAL, manœuvre…)
 *   - une position (index dans le tableau des signaux du canton)
 *   - un nombre de feux (m_length)
 *   - des capacités (présence d’un carré, d’un œilleton, d’un rappel, etc.)
 *
 * Le but de ce module :
 *   - Décrire les capacités d’un signal selon son type
 *   - Permettre au SA d’envoyer les bons aspects à l’EXSA
 *   - Préparer l’évolution vers des signaux plus complexes (BAL, entrée, sortie)
 */

#include "Signal.h"
#include "debug_sa.h"

/*
 * Constructeur
 * ---------------------------------------------------------------------------
 * Initialise toutes les capacités à false.
 * Le type réel sera défini plus tard via type().
 */
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
    SA_LOG_TRACE("[Signal] Constructeur : type=%u position=%u\n",
                 m_type, m_position);
}

Signal::~Signal() {}

/*
 * setup()
 * ---------------------------------------------------------------------------
 * Configure les capacités du signal en fonction de son type.
 *
 * Rappel : un signal SNCF n’a pas toujours les mêmes feux.
 * Exemple :
 *   - un signal simple → 3 feux (Rouge / Jaune / Vert)
 *   - un carré → ajoute un feu carré + un œilleton
 *   - un ralentissement → 3×3 feux (RAL 30 / 60 / 90)
 *   - un rappel → 3×3 feux (Rappel 30 / 60 / 90)
 *   - un signal de manœuvre → Blanc + Violet
 *
 * Ce module ne gère PAS l’allumage des feux, seulement la description.
 */
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

    SA_LOG_TRACE("[Signal] setup() : configuration du type %u\n", m_type);

    switch (m_type)
    {
        // -----------------------------------------------------------------------
        // Types existants (compatibilité Exploration)
        // -----------------------------------------------------------------------

    case SIG_SIMPLE: // 0
        /*
         * Signal simple :
         *   - Rouge
         *   - Jaune
         *   - Vert
         */
        m_length = 3;
        SA_LOG_INFO("[Signal] Type SIMPLE → 3 feux\n");
        break;

    case SIG_CARRE: // 1
        /*
         * Carré :
         *   - Rouge
         *   - Jaune
         *   - Vert
         *   - Carré (RR)
         *   - Œilleton
         */
        m_length = 5;
        m_hasCarre = true;
        m_hasOeilleton = true;
        SA_LOG_INFO("[Signal] Type CARRE → 5 feux (Carré + Œilleton)\n");
        break;

    case SIG_RAL: // 2
        /*
         * Ralentissement :
         *   - 3 feux principaux (R/J/V)
         *   - 3 feux RAL 30
         *   - 3 feux RAL 60
         */
        m_length = 9;
        m_hasRal = true;
        SA_LOG_INFO("[Signal] Type RAL → 9 feux (RAL 30/60)\n");
        break;

    case SIG_RAPPEL: // 3
        /*
         * Rappel de ralentissement :
         *   - 3 feux principaux
         *   - 3 feux Rappel 30
         *   - 3 feux Rappel 60
         */
        m_length = 9;
        m_hasRappel = true;
        SA_LOG_INFO("[Signal] Type RAPPEL → 9 feux (Rappel 30/60)\n");
        break;

        // -----------------------------------------------------------------------
        // Nouveaux types (préparés pour l’évolution Exploration 2026)
        // -----------------------------------------------------------------------

    case SIG_MANOEUVRE: // 4
        /*
         * Signal de manœuvre :
         *   - Blanc
         *   - Violet (Carré violet)
         */
        m_length = 2;
        m_hasManoeuvre = true;
        m_hasCarreViolet = true;
        SA_LOG_INFO("[Signal] Type MANOEUVRE → Blanc + Violet\n");
        break;

    case SIG_BAL: // 5
        /*
         * Signal BAL (Block Automatique Lumineux) :
         *   - Rouge
         *   - Jaune
         *   - Vert clignotant
         */
        m_length = 3;
        m_hasVLclignotant = true;
        SA_LOG_INFO("[Signal] Type BAL → Rouge / Jaune / Vert clignotant\n");
        break;

    case SIG_ENTREE: // 6
        /*
         * Signal d’entrée de gare :
         *   - souvent 3×3 feux
         *   - carré + ralentissements
         */
        m_length = 9;
        m_hasCarre = true;
        m_hasRal = true;
        SA_LOG_INFO("[Signal] Type ENTREE → Carré + RAL\n");
        break;

    case SIG_SORTIE: // 7
        /*
         * Signal de sortie de gare :
         *   - souvent 3×3 feux
         *   - rappel de ralentissement
         */
        m_length = 9;
        m_hasRappel = true;
        SA_LOG_INFO("[Signal] Type SORTIE → Rappel\n");
        break;

    default:
        /*
         * Type inconnu → fallback sur un signal simple
         */
        m_length = 3;
        SA_LOG_WARN("[Signal] Type inconnu (%u) → fallback SIMPLE\n", m_type);
        break;
    }

    SA_LOG_TRACE("[Signal] setup() terminé : length=%u\n", m_length);
}

/*
 * Setters / Getters
 * ---------------------------------------------------------------------------
 * type()      → type du signal (SIG_SIMPLE, SIG_CARRE, etc.)
 * position()  → index du signal dans le canton
 * length()    → nombre total de feux gérés par ce signal
 */

void Signal::type(uint8_t t)
{
    m_type = t;
    SA_LOG_TRACE("[Signal] type() = %u\n", t);
}

uint8_t Signal::type() const { return m_type; }

void Signal::position(uint8_t p)
{
    m_position = p;
    SA_LOG_TRACE("[Signal] position() = %u\n", p);
}

uint8_t Signal::position() const { return m_position; }

uint8_t Signal::length() const { return m_length; }
