#pragma once
#include <Arduino.h>

/*
 * CC_RS485.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Couche bas-niveau RS485 pour la communication CC ↔ EXCC.
 *
 * Rôle :
 *   - initialiser l’UART RS485
 *   - gérer la direction du transceiver (DE/RE)
 *   - envoyer des octets ou des trames
 *   - fournir un readByte() non bloquant
 *
 * Ce module ne contient aucune logique métier :
 *   → il transporte uniquement les données brutes.
 */

namespace CC_RS485
{
    // -----------------------------------------------------------------------
    // Initialisation RS485
    // -----------------------------------------------------------------------
    void begin();

    // -----------------------------------------------------------------------
    // Gestion direction transceiver
    // -----------------------------------------------------------------------
    void setTx();
    void setRx();

    // -----------------------------------------------------------------------
    // Envoi
    // -----------------------------------------------------------------------
    void sendByte(uint8_t b);
    void sendFrame(const uint8_t *data, size_t len);

    // -----------------------------------------------------------------------
    // Lecture non bloquante
    // -----------------------------------------------------------------------
    int readByte();
}
