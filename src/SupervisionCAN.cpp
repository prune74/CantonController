/*
 * SupervisionCAN.cpp — Supervision du CAN Exploration 2026
 */

#include "SupervisionCAN.h"
#include "CanMsg.h"
#include "debug_sa.h"
#include "Canton.h"

void envoyerEtatCAN(Canton *canton)
{
    // Récupération des voisins via l’API moderne
    CantonPeriph *sp1 = canton->getCantonP(canton->SP1_idx());
    CantonPeriph *sm1 = canton->getCantonP(canton->SM1_idx());

    // Sécurité : si la topologie n’est pas encore prête
    if (!sp1 || !sm1)
    {
        SA_LOG_WARN("[CAN] Topologie incomplète → trame 0xE0 ignorée\n");
        return;
    }

    // Log pédagogique
    SA_LOG_TRACE(
        "[CAN] Envoi 0xE0 : busy=%d SP1=%d SM1=%d accesSP1=%d busySP1=%d accesSM1=%d busySM1=%d\n",
        canton->busy(),
        sp1->ID(),
        sm1->ID(),
        sp1->acces(),
        sp1->busy(),
        sm1->acces(),
        sm1->busy());

    // Envoi trame 0xE0
    CanMsg::sendMsg(
        0, 0xE0, 0, canton->ID(),
        canton->busy(),
        static_cast<uint8_t>(sp1->ID()),
        static_cast<uint8_t>(sm1->ID()),
        sp1->acces(),
        sp1->busy(),
        sm1->acces(),
        sm1->busy());

    SA_LOG_INFO("[CAN] Trame 0xE0 envoyée pour Canton %d\n", canton->ID());

    /*
     * =========================================================================
     *  TRAME 0xE3 — Réservation du canton suivant
     * =========================================================================
     */

    Loco *loco = canton->getLoco();
    if (!loco)
        return;

    uint16_t addr = loco->address();
    if (addr == 0)
    {
        SA_LOG_TRACE("[CAN] Pas de loco → trame 0xE3 non envoyée\n");
        return;
    }

    uint8_t addrHigh = (addr >> 8) & 0xFF;
    uint8_t addrLow = addr & 0xFF;

    SensDeMarche sens = loco->sens();

    // Sens horaire → SP1
    if (sens == SensHoraire)
    {
        uint8_t aval = sp1->ID();

        SA_LOG_INFO("[CAN] Envoi 0xE3 (horaire) : aval=%d loco=%u\n",
                    aval, addr);

        CanMsg::sendMsg(
            0, 0xE3, 0, canton->ID(),
            aval,
            addrHigh, addrLow);
    }
    // Sens anti-horaire → SM1
    else if (sens == SensAntiHoraire)
    {
        uint8_t aval = sm1->ID();

        SA_LOG_INFO("[CAN] Envoi 0xE3 (anti-horaire) : aval=%d loco=%u\n",
                    aval, addr);

        CanMsg::sendMsg(
            0, 0xE3, 0, canton->ID(),
            aval,
            addrHigh, addrLow);
    }
    else
    {
        SA_LOG_WARN("[CAN] Loco présente mais sens invalide (%d) → trame 0xE3 ignorée\n",
                    static_cast<int>(sens));
    }
}
