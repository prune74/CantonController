/*
 * SupervisionCAN.cpp — Gestion Canton 2026
 * ------------------------------------------------------------
 * Supervision et émission des trames CAN pour le
 * Canton Controller (CC).
 */

#include "SupervisionCAN.h"
#include "CC_CAN.h"
#include "debug_cc.h"
#include "Config.h"
#include "Canton.h"

/* ============================================================================
 * updateTopoLed() — LED topologie valide / invalide
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
 * ==========================================================================*/
void envoyerEtatCAN(Canton *canton)
{
    // Récupération des voisins via l’API moderne
    CantonPeriph *sp1 = canton->getCantonP(canton->SP1_idx());
    CantonPeriph *sm1 = canton->getCantonP(canton->SM1_idx());

    // Sécurité : topologie non prête
    if (!sp1 || !sm1)
    {
        CC_LOG_WARN("[CAN][CC] Topologie incomplète → trame UPDATE_VOISINS ignorée\n");
        return;
    }

    // Log pédagogique
    CC_LOG_TRACE(
        "[CAN][CC] Envoi UPDATE_VOISINS : busy=%d SP1=%d SM1=%d accesSP1=%d busySP1=%d accesSM1=%d busySM1=%d\n",
        canton->busy(),
        sp1->ID(),
        sm1->ID(),
        sp1->acces(),
        sp1->busy(),
        sm1->acces(),
        sm1->busy());

    // -----------------------------------------------------------------------
    // TRAME UPDATE_VOISINS — État ferroviaire du canton
    // -----------------------------------------------------------------------
    CC_CAN::sendMsg(
        0,
        (uint16_t)Cmd_CC_to_CC::UPDATE_VOISINS,
        0,
        canton->ID(),
        canton->busy(),
        (uint8_t)sp1->ID(),
        (uint8_t)sm1->ID(),
        sp1->acces(),
        sp1->busy(),
        sm1->acces(),
        sm1->busy());

    CC_LOG_INFO("[CAN][CC] Trame UPDATE_VOISINS envoyée pour Canton %d\n", canton->ID());

    // -----------------------------------------------------------------------
    // TRAME RESERVATION_LOCO — Réservation du canton suivant
    // -----------------------------------------------------------------------
    Loco *loco = canton->getLoco();
    if (!loco)
        return;

    uint16_t addr = loco->address();
    if (addr == 0)
    {
        CC_LOG_TRACE("[CAN][CC] Pas de loco → trame RESERVATION_LOCO non envoyée\n");
        return;
    }

    uint8_t addrHigh = (addr >> 8) & 0xFF;
    uint8_t addrLow = addr & 0xFF;

    SensDeMarche sens = loco->sens();

    // Sens horaire → SP1
    if (sens == SensHoraire)
    {
        uint8_t aval = sp1->ID();

        CC_LOG_INFO("[CAN][CC] Envoi RESERVATION_LOCO (horaire) : aval=%d loco=%u\n",
                    aval, addr);

        CC_CAN::sendMsg(
            0,
            (uint16_t)Cmd_CC_to_CC::RESERVATION_LOCO,
            0,
            canton->ID(),
            aval,
            addrHigh, addrLow);
    }
    // Sens anti‑horaire → SM1
    else if (sens == SensAntiHoraire)
    {
        uint8_t aval = sm1->ID();

        CC_LOG_INFO("[CAN][CC] Envoi RESERVATION_LOCO (anti-horaire) : aval=%d loco=%u\n",
                    aval, addr);

        CC_CAN::sendMsg(
            0,
            (uint16_t)Cmd_CC_to_CC::RESERVATION_LOCO,
            0,
            canton->ID(),
            aval,
            addrHigh, addrLow);
    }
    else
    {
        CC_LOG_WARN("[CAN][CC] Loco présente mais sens invalide (%d) → trame RESERVATION_LOCO ignorée\n",
                    (int)sens);
    }
}
