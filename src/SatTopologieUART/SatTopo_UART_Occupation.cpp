/*
 * SatTopo_UART_Occupation.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Transmission de l’occupation des cantons voisins vers les EXCC via UART.
 *
 * OpCode :
 *   - EA : occupation SP1 / SM1 (2 bits)
 *
 * Rôle :
 *   - lire l’état d’occupation des deux voisins directs :
 *        • SP1 (sens horaire)
 *        • SM1 (sens anti‑horaire)
 *   - encoder ces deux états dans un octet compact :
 *        bit 1 = SP1 occupé
 *        bit 0 = SM1 occupé
 *   - transmettre cet octet aux EXCC
 *
 * IMPORTANT :
 *   - aucune logique métier ici
 *   - aucune lecture de capteurs physiques
 *   - aucune interprétation RailCom / essieux
 *
 * Toute la logique d’occupation est gérée dans CantonPeriph et Occupation.
 */

#include "SatTopologieUART.h"
#include "Config.h"
#include "Exploration_Protocol.h"
#include "debug_cc.h"

#include "Settings.h"
#include "Canton.h"

extern HardwareSerial Serial1;

/* ============================================================================
 *  envoyerOccupationDepuisEtatCourant()
 * ---------------------------------------------------------------------------
 *  Lit l’occupation SP1 / SM1 et envoie l’octet EA correspondant.
 * ==========================================================================*/
void envoyerOccupationDepuisEtatCourant() // 🟢
{
    uint8_t occSP1 = 0;
    uint8_t occSM1 = 0;

    CantonPeriph *sp1 = Settings::canton->getCantonP(Settings::canton->SP1_idx());
    CantonPeriph *sm1 = Settings::canton->getCantonP(Settings::canton->SM1_idx());

    if (sp1 && sp1->busy())
        occSP1 = 1;

    if (sm1 && sm1->busy())
        occSM1 = 1;

    uint8_t valeur = (occSP1 << 1) | occSM1;

    envoyerOccupationVoisins(valeur);

    CC_LOG_INFO("[TopoUART][CC] EA envoyé : SP1=%u SM1=%u (valeur=%u)\n",
                occSP1, occSM1, valeur);
}

/* ============================================================================
 *  envoyerOccupationVoisins() — opcode EA
 * ==========================================================================*/
void envoyerOccupationVoisins(uint8_t valeur) // 🟢
{
    CC_LOG_TRACE("[TopoUART][CC] Occupation voisins (EA) = %u\n", valeur);

    Serial1.write(PROTO_SYNC_BYTE);
    Serial1.write(PROTO_EA_OCCUPATION_VOISINS);
    Serial1.write(valeur);
}
