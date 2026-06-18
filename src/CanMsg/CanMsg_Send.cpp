/*
 * CanMsg_Send.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Fonctions d’envoi CAN (Exploration 2026)
 *
 * Rôle :
 *   - fournir un point unique d’envoi de trames CAN
 *   - encapsuler le format 29 bits Exploration 2026
 *   - proposer une API simple et lisible :
 *
 *       CanMsg::sendMsg(prio, cmde, resp, id, data...)
 *
 * Format ID étendu (29 bits) :
 *
 *      [ 2 bits priorité ]   bits 26..25
 *      [ 8 bits commande ]   bits 24..17
 *      [ 1 bit response ]    bit  16
 *      [ 16 bits ID SA ]     bits 15..0
 *
 * Exemple :
 *      prio=1, cmde=0xE7, resp=0, id=42
 *      → ID = (1<<25) | (0xE7<<17) | (0<<16) | 42
 *
 * IMPORTANT :
 *   - aucune logique métier ici
 *   - aucune logique ferroviaire
 *   - ce module ne fait que construire et envoyer des trames CAN
 */

#include "CanMsg.h"

/* ============================================================================
 * formatMsg() — Construction de l’ID étendu Exploration 2026
 * ---------------------------------------------------------------------------
 * Fonction interne (namespace anonyme) :
 *   - assemble les champs prio / cmde / resp / id
 *   - force frame.ext = true
 * ==========================================================================*/
namespace
{
    CANMessage &formatMsg(CANMessage &frame,
                          byte prio, byte cmde, byte resp,
                          uint16_t thisCantonId)
    {
        frame.id  = 0;
        frame.id |= (uint32_t)prio << 25;        // Priorité (0=haute)
        frame.id |= (uint32_t)cmde << 17;        // Commande
        frame.id |= (uint32_t)resp << 16;        // Bit réponse
        frame.id |= (uint32_t)thisCantonId;      // ID CC expéditeur

        frame.ext = true;                        // ID étendu obligatoire
        return frame;
    }
}

/* ============================================================================
 * Envoi brut : sendMsg(CANMessage&)
 * ---------------------------------------------------------------------------
 * Envoie une trame CAN déjà construite.
 * ==========================================================================*/
void CanMsg::sendMsg(CANMessage &frame)
{
    /*
      tryToSend() :
        - renvoie 0 si échec (bus saturé)
        - renvoie 1 si OK

      Les trames Exploration étant périodiques, un échec ponctuel n’est pas critique.
    */
    ACAN_ESP32::can.tryToSend(frame);
}

/* ============================================================================
 * Surcharges sendMsg() — API simplifiée
 * ---------------------------------------------------------------------------
 * Toutes les variantes ci‑dessous :
 *   - construisent un CANMessage
 *   - appellent formatMsg()
 *   - remplissent frame.data[]
 *   - appellent sendMsg(frame)
 *
 * Longueur : 0 à 8 octets
 * ==========================================================================*/

// --- 0 octet ----------------------------------------------------------------
void CanMsg::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId)
{
    CANMessage frame;
    formatMsg(frame, prio, cmde, resp, thisCantonId);
    frame.len = 0;
    sendMsg(frame);
}

// --- 1 octet ----------------------------------------------------------------
void CanMsg::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId,
                     byte d0)
{
    CANMessage frame;
    formatMsg(frame, prio, cmde, resp, thisCantonId);
    frame.len = 1;
    frame.data[0] = d0;
    sendMsg(frame);
}

// --- 2 octets ---------------------------------------------------------------
void CanMsg::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId,
                     byte d0, byte d1)
{
    CANMessage frame;
    formatMsg(frame, prio, cmde, resp, thisCantonId);
    frame.len = 2;
    frame.data[0] = d0;
    frame.data[1] = d1;
    sendMsg(frame);
}

// --- 3 octets ---------------------------------------------------------------
void CanMsg::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId,
                     byte d0, byte d1, byte d2)
{
    CANMessage frame;
    formatMsg(frame, prio, cmde, resp, thisCantonId);
    frame.len = 3;
    frame.data[0] = d0;
    frame.data[1] = d1;
    frame.data[2] = d2;
    sendMsg(frame);
}

// --- 4 octets ---------------------------------------------------------------
void CanMsg::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId,
                     byte d0, byte d1, byte d2, byte d3)
{
    CANMessage frame;
    formatMsg(frame, prio, cmde, resp, thisCantonId);
    frame.len = 4;
    frame.data[0] = d0;
    frame.data[1] = d1;
    frame.data[2] = d2;
    frame.data[3] = d3;
    sendMsg(frame);
}

// --- 5 octets ---------------------------------------------------------------
void CanMsg::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId,
                     byte d0, byte d1, byte d2, byte d3, byte d4)
{
    CANMessage frame;
    formatMsg(frame, prio, cmde, resp, thisCantonId);
    frame.len = 5;
    frame.data[0] = d0;
    frame.data[1] = d1;
    frame.data[2] = d2;
    frame.data[3] = d3;
    frame.data[4] = d4;
    sendMsg(frame);
}

// --- 6 octets ---------------------------------------------------------------
void CanMsg::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId,
                     byte d0, byte d1, byte d2, byte d3, byte d4, byte d5)
{
    CANMessage frame;
    formatMsg(frame, prio, cmde, resp, thisCantonId);
    frame.len = 6;
    frame.data[0] = d0;
    frame.data[1] = d1;
    frame.data[2] = d2;
    frame.data[3] = d3;
    frame.data[4] = d4;
    frame.data[5] = d5;
    sendMsg(frame);
}

// --- 7 octets ---------------------------------------------------------------
void CanMsg::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId,
                     byte d0, byte d1, byte d2, byte d3, byte d4, byte d5, byte d6)
{
    CANMessage frame;
    formatMsg(frame, prio, cmde, resp, thisCantonId);
    frame.len = 7;
    frame.data[0] = d0;
    frame.data[1] = d1;
    frame.data[2] = d2;
    frame.data[3] = d3;
    frame.data[4] = d4;
    frame.data[5] = d5;
    frame.data[6] = d6;
    sendMsg(frame);
}

// --- 8 octets ---------------------------------------------------------------
void CanMsg::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId,
                     byte d0, byte d1, byte d2, byte d3,
                     byte d4, byte d5, byte d6, byte d7)
{
    CANMessage frame;
    formatMsg(frame, prio, cmde, resp, thisCantonId);
    frame.len = 8;
    frame.data[0] = d0;
    frame.data[1] = d1;
    frame.data[2] = d2;
    frame.data[3] = d3;
    frame.data[4] = d4;
    frame.data[5] = d5;
    frame.data[6] = d6;
    frame.data[7] = d7;
    sendMsg(frame);
}
