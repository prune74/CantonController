/*
 * ============================================================
 *  ConsoCourant.cpp — SA 2026 (version centralisée)
 * ============================================================
 *
 * Trame reçue :
 *   [SYNC][0x04][exsaAdresse][code]
 *
 * exsaAdresse :
 *   0 = EXSA Horaire
 *   1 = EXSA AntiHoraire
 *
 * Un SA = un canton.
 * Donc cette information concerne TOUJOURS le canton du SA.
 */

#include "ConsoCourant.h"
#include "Discovery_Protocol.h"
#include "CompteurEssieuxUart.h"
#include "Node.h"
#include "debug_sa.h"

/* ============================================================
   Singleton (instance unique du SA)
   ============================================================ */
ConsoCourant* ConsoCourant::s_instance = nullptr;

/* ============================================================
   Constructeur / Destructeur
   ============================================================ */
ConsoCourant::ConsoCourant() {}
ConsoCourant::~ConsoCourant() {}

/* ============================================================
   setup()
   ============================================================ */
void ConsoCourant::setup(Node *node)
{
    m_node = node;
    s_instance = this;   // <-- instance unique
}

/* ============================================================
   updateEtat(occupePhysique)
   ============================================================ */
void ConsoCourant::updateEtat(bool occupePhysique)
{
    if (!m_node)
    {
        SA_LOG_ERROR("[ConsoCourant] Erreur : m_node nul\n");
        return;
    }

    int compteur = CompteurEssieuxUart::compteurGlobal();
    bool occupeLogique = occupePhysique || (compteur > 0);

    m_node->busy(occupeLogique);

    SA_LOG_INFO(
        "[ConsoCourant] Canton = %s (phys=%d, essieux=%d)\n",
        occupeLogique ? "OCCUPE" : "LIBRE",
        occupePhysique ? 1 : 0,
        compteur);
}

/* ============================================================
   onOccupation() — callback appelé par SA_UartRx
   ============================================================ */
void ConsoCourant::onOccupation(uint8_t index_exsa, uint8_t code)
{
    bool occPhys = false;

    switch (code)
    {
    case PROTO_OCC_ACTIVE:
        occPhys = true;
        break;

    case PROTO_OCC_LIBRE:
        occPhys = false;
        break;

    default:
        SA_LOG_WARN("[ConsoCourant] Code occupation inconnu : %02X\n", code);
        return;
    }

    // ============================================================
    // Utilisation du singleton (un SA = un seul ConsoCourant)
    // ============================================================
    if (!s_instance)
    {
        SA_LOG_ERROR("[ConsoCourant] s_instance nul dans onOccupation()\n");
        return;
    }

    s_instance->updateEtat(occPhys);
}
