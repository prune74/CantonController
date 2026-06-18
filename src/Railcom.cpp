/*
 * Railcom.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Réception de l’adresse RailCom détectée par l’Extension Canton Controller
 * (EXCC) et transmise au Canton Controller (CC).
 *
 * Rôle :
 *   - stocker la dernière adresse RailCom reçue
 *   - fournir une API simple : begin(), address(), onRailcom()
 *
 * Ce module ne fait aucun calcul : il reflète simplement l’information
 * envoyée par EXCC.
 */

#include "Railcom.h"
#include "debug_cc.h"

// ---------------------------------------------------------------------------
// Variable statique : dernière adresse RailCom reçue
// ---------------------------------------------------------------------------
volatile uint16_t Railcom::s_address = 0;

// ---------------------------------------------------------------------------
// Initialisation
// ---------------------------------------------------------------------------
void Railcom::begin()
{
    s_address = 0;
    CC_LOG_INFO("[Railcom][CC] Initialisé (réception EXCC → CC)\n");
}

// ---------------------------------------------------------------------------
// Lecture de l’adresse RailCom courante
// ---------------------------------------------------------------------------
uint16_t Railcom::address()
{
    return s_address;
}

// ---------------------------------------------------------------------------
// Callback : réception d’une adresse RailCom depuis EXCC
// ---------------------------------------------------------------------------
void Railcom::onRailcom(uint8_t index_excc, uint8_t low, uint8_t high)
{
    uint16_t adr = (uint16_t(high) << 8) | low;
    s_address = adr;

    CC_LOG_INFO("[Railcom][CC] EXCC %u → adresse RailCom = %u\n",
                index_excc, adr);
}
