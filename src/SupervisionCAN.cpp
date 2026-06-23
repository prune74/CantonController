/*
 * SupervisionCAN.cpp — Gestion Canton 2026
 * ------------------------------------------------------------
 * Supervision et émission des trames CAN pour le
 * Canton Controller (CC).
 *
 * Rôle :
 *   - transmettre l’état ferroviaire local au réseau CAN
 *   - informer l’EXCC et les cantons voisins :
 *       • occupation locale
 *       • accessibilité SP1 / SM1
 *       • occupation SP1 / SM1
 *   - transmettre la réservation locomotive (trame 0xE3)
 *
 * Trames :
 *   - 0xE0 : état ferroviaire du canton
 *   - 0xE3 : réservation du canton suivant selon le sens
 */

#include "SupervisionCAN.h"
#include "CanMsg.h"
#include "debug_cc.h"
#include "Config.h"
#include "Canton.h"

/* ============================================================================
 * updateTopoLed() — LED topologie valide / invalide
 * ---------------------------------------------------------------------------
 * Utilise le MCP23017 local pour afficher l’état topologique :
 *
 *   - topoValide = true  → LED éteinte
 *   - topoValide = false → LED allumée
 *
 * IMPORTANT :
 *   - aucune logique métier
 *   - aucune logique ferroviaire
 *   - simple indicateur visuel
 * ==========================================================================*/
void updateTopoLed()
{
    Canton *c = Canton::s_instance;
    if (!c)
        return;

    c->mcp.pinMode(MCP_PIN_LED_TOPOLOGIE, OUTPUT);
    c->mcp.digitalWrite(MCP_PIN_LED_TOPOLOGIE, Canton::topoValide ? LOW : HIGH);

    CC_LOG_INFO("[CAN][Supervision][CC] LED topo = %s\n",
                Canton::topoValide ? "OK" : "ERREUR");
}

/* ============================================================================
 * envoyerEtatCAN()
 * ---------------------------------------------------------------------------
 * Envoie les trames 0xE0 et 0xE3 selon l’état du canton.
 * ==========================================================================*/
void envoyerEtatCAN(Canton *canton)
{
    // Récupération des voisins via l’API moderne
    CantonPeriph *sp1 = canton->getCantonP(canton->SP1_idx());
    CantonPeriph *sm1 = canton->getCantonP(canton->SM1_idx());

    // Sécurité : topologie non prête
    if (!sp1 || !sm1)
    {
        CC_LOG_WARN("[CAN][CC] Topologie incomplète → trame 0xE0 ignorée\n");
        return;
    }

    // Log pédagogique
    CC_LOG_TRACE(
        "[CAN][CC] Envoi 0xE0 : busy=%d SP1=%d SM1=%d accesSP1=%d busySP1=%d accesSM1=%d busySM1=%d\n",
        canton->busy(),
        sp1->ID(),
        sm1->ID(),
        sp1->acces(),
        sp1->busy(),
        sm1->acces(),
        sm1->busy());

    // -----------------------------------------------------------------------
    // TRAME 0xE0 — État ferroviaire du canton
    // -----------------------------------------------------------------------
    CanMsg::sendMsg(
        0, 0xE0, 0, canton->ID(),
        canton->busy(),
        static_cast<uint8_t>(sp1->ID()),
        static_cast<uint8_t>(sm1->ID()),
        sp1->acces(),
        sp1->busy(),
        sm1->acces(),
        sm1->busy());

    CC_LOG_INFO("[CAN][CC] Trame 0xE0 envoyée pour Canton %d\n", canton->ID());

    // -----------------------------------------------------------------------
    // TRAME 0xE3 — Réservation du canton suivant
    // -----------------------------------------------------------------------
    Loco *loco = canton->getLoco();
    if (!loco)
        return;

    uint16_t addr = loco->address();
    if (addr == 0)
    {
        CC_LOG_TRACE("[CAN][CC] Pas de loco → trame 0xE3 non envoyée\n");
        return;
    }

    uint8_t addrHigh = (addr >> 8) & 0xFF;
    uint8_t addrLow = addr & 0xFF;

    SensDeMarche sens = loco->sens();

    // Sens horaire → SP1
    if (sens == SensHoraire)
    {
        uint8_t aval = sp1->ID();

        CC_LOG_INFO("[CAN][CC] Envoi 0xE3 (horaire) : aval=%d loco=%u\n",
                    aval, addr);

        CanMsg::sendMsg(
            0, 0xE3, 0, canton->ID(),
            aval,
            addrHigh, addrLow);
    }
    // Sens anti‑horaire → SM1
    else if (sens == SensAntiHoraire)
    {
        uint8_t aval = sm1->ID();

        CC_LOG_INFO("[CAN][CC] Envoi 0xE3 (anti-horaire) : aval=%d loco=%u\n",
                    aval, addr);

        CanMsg::sendMsg(
            0, 0xE3, 0, canton->ID(),
            aval,
            addrHigh, addrLow);
    }
    else
    {
        CC_LOG_WARN("[CAN][CC] Loco présente mais sens invalide (%d) → trame 0xE3 ignorée\n",
                    static_cast<int>(sens));
    }
}
