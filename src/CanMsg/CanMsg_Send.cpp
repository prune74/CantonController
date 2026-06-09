/*
  CanMsg_Send.cpp — Fonctions d’envoi CAN (Version 2026)
  --------------------------------------------------------------------------
  Ce module regroupe TOUTES les fonctions d’envoi de trames CAN.

  Il fournit :
    - sendMsg(CANMessage&) : envoi brut
    - sendMsg(prio, cmde, resp, id, data...) : API simplifiée

  Le format CAN Exploration 2026 utilise un ID étendu (29 bits) structuré ainsi :

      [ 2 bits priorité ]   (bits 26..25)
      [ 8 bits commande ]   (bits 24..17)
      [ 1 bit response ]    (bit  16)
      [ 16 bits ID SA ]     (bits 15..0)

  Exemple :
      prio = 1
      commande = 0xE7
      response = 0
      id = 42

      ID = (1 << 25) | (0xE7 << 17) | (0 << 16) | 42

  Toutes les fonctions sendMsg() utilisent ce format.
*/

#include "CanMsg.h"

// ============================================================================
//  Fonction interne : formatMsg()
//  --------------------------------------------------------------------------
//  Construit l’ID étendu CAN selon le protocole Exploration 2026.
//  Cette fonction n’est PAS exposée dans le .h (namespace anonyme).
// ============================================================================
namespace
{
  CANMessage &formatMsg(CANMessage &frame, byte prio, byte cmde, byte resp, uint16_t thisCantonId)
  {
    /*
      Construction de l’ID étendu :

        frame.id =
            [prio << 25]   (2 bits)
          | [cmde << 17]   (8 bits)
          | [resp << 16]   (1 bit)
          | [thisCantonId]   (16 bits)
    */

    frame.id = 0;
    frame.id |= (uint32_t)prio << 25;   // Priorité (0 = haute, 1 = moyenne, 2 = basse)
    frame.id |= (uint32_t)cmde << 17;   // Commande CAN
    frame.id |= (uint32_t)resp << 16;   // Bit "response"
    frame.id |= (uint32_t)thisCantonId; // ID du SA expéditeur

    frame.ext = true; // ID étendu obligatoire pour Exploration
    return frame;
  }
}

// ============================================================================
//  Envoi brut : sendMsg(CANMessage&)
//  --------------------------------------------------------------------------
//  Envoie une trame CAN déjà formatée.
//  Utilisé par toutes les surcharges ci‑dessous.
// ============================================================================
void CanMsg::sendMsg(CANMessage &frame)
{
  /*
    tryToSend() :
      - renvoie 0 si échec (bus saturé)
      - renvoie 1 si OK

    Ici on ignore le retour car :
      - les trames Exploration sont périodiques
      - un échec ponctuel n’est pas critique
  */
  ACAN_ESP32::can.tryToSend(frame);
}

// ============================================================================
//  Surcharges sendMsg() — API simplifiée
//  --------------------------------------------------------------------------
//  Ces fonctions permettent d’envoyer une trame CAN en écrivant simplement :
//
//      CanMsg::sendMsg(1, 0xE7, 0, canton->ID(), aspect);
//
//  Au lieu de construire manuellement un CANMessage.
// ============================================================================

// --- 0 octet de données -----------------------------------------------------
void CanMsg::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId)
{
  CANMessage frame;
  formatMsg(frame, prio, cmde, resp, thisCantonId);
  frame.len = 0;
  CanMsg::sendMsg(frame);
}

// --- 1 octet de données -----------------------------------------------------
void CanMsg::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId, byte data0)
{
  CANMessage frame;
  formatMsg(frame, prio, cmde, resp, thisCantonId);
  frame.len = 1;
  frame.data[0] = data0;
  CanMsg::sendMsg(frame);
}

// --- 2 octets de données ----------------------------------------------------
void CanMsg::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId,
                     byte data0, byte data1)
{
  CANMessage frame;
  formatMsg(frame, prio, cmde, resp, thisCantonId);
  frame.len = 2;
  frame.data[0] = data0;
  frame.data[1] = data1;
  CanMsg::sendMsg(frame);
}

// --- 3 octets de données ----------------------------------------------------
void CanMsg::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId,
                     byte data0, byte data1, byte data2)
{
  CANMessage frame;
  formatMsg(frame, prio, cmde, resp, thisCantonId);
  frame.len = 3;
  frame.data[0] = data0;
  frame.data[1] = data1;
  frame.data[2] = data2;
  CanMsg::sendMsg(frame);
}

// --- 4 octets de données ----------------------------------------------------
void CanMsg::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId,
                     byte data0, byte data1, byte data2, byte data3)
{
  CANMessage frame;
  formatMsg(frame, prio, cmde, resp, thisCantonId);
  frame.len = 4;
  frame.data[0] = data0;
  frame.data[1] = data1;
  frame.data[2] = data2;
  frame.data[3] = data3;
  CanMsg::sendMsg(frame);
}

// --- 5 octets de données ----------------------------------------------------
void CanMsg::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId,
                     byte data0, byte data1, byte data2, byte data3, byte data4)
{
  CANMessage frame;
  formatMsg(frame, prio, cmde, resp, thisCantonId);
  frame.len = 5;
  frame.data[0] = data0;
  frame.data[1] = data1;
  frame.data[2] = data2;
  frame.data[3] = data3;
  frame.data[4] = data4;
  CanMsg::sendMsg(frame);
}

// --- 6 octets de données ----------------------------------------------------
void CanMsg::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId,
                     byte data0, byte data1, byte data2, byte data3,
                     byte data4, byte data5)
{
  CANMessage frame;
  formatMsg(frame, prio, cmde, resp, thisCantonId);
  frame.len = 6;
  frame.data[0] = data0;
  frame.data[1] = data1;
  frame.data[2] = data2;
  frame.data[3] = data3;
  frame.data[4] = data4;
  frame.data[5] = data5;
  CanMsg::sendMsg(frame);
}

// --- 7 octets de données ----------------------------------------------------
void CanMsg::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId,
                     byte data0, byte data1, byte data2, byte data3,
                     byte data4, byte data5, byte data6)
{
  CANMessage frame;
  formatMsg(frame, prio, cmde, resp, thisCantonId);
  frame.len = 7;
  frame.data[0] = data0;
  frame.data[1] = data1;
  frame.data[2] = data2;
  frame.data[3] = data3;
  frame.data[4] = data4;
  frame.data[5] = data5;
  frame.data[6] = data6;
  CanMsg::sendMsg(frame);
}

// --- 8 octets de données ----------------------------------------------------
void CanMsg::sendMsg(byte prio, byte cmde, byte resp, uint16_t thisCantonId,
                     byte data0, byte data1, byte data2, byte data3,
                     byte data4, byte data5, byte data6, byte data7)
{
  CANMessage frame;
  formatMsg(frame, prio, cmde, resp, thisCantonId);
  frame.len = 8;
  frame.data[0] = data0;
  frame.data[1] = data1;
  frame.data[2] = data2;
  frame.data[3] = data3;
  frame.data[4] = data4;
  frame.data[5] = data5;
  frame.data[6] = data6;
  frame.data[7] = data7;
  CanMsg::sendMsg(frame);
}
