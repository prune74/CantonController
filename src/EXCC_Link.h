#pragma once
#include <stdint.h>

/*
 * EXCC_Link.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Interface publique de la couche de communication RS485 entre :
 *   - le Canton Controller (CC)
 *   - l’Extension Canton Controller (EXCC)
 *
 * Rôle :
 *   - supervision ONLINE / OFFLINE des EXCC
 *   - gestion du protocole UART (PING / PONG)
 *   - gestion du booster (état, tension, courant, présence)
 *   - envoi des configurations (topologie, signaux, servos…)
 *   - envoi des seuils calibrés (F4)
 *   - commande ON/OFF du booster (F5)
 *
 * Ce module ne contient aucune logique ferroviaire :
 *   → il transporte uniquement les données CC ↔ EXCC.
 */

class EXCC_Link
{
public:
    // -----------------------------------------------------------------------
    // Initialisation + boucle principale
    // -----------------------------------------------------------------------
    static void begin();
    static void loop();

    // -----------------------------------------------------------------------
    // Supervision EXCC
    // -----------------------------------------------------------------------
    static bool isOnline(uint8_t index);
    static void onPong(uint8_t index);
    static void onExccOnline(uint8_t index);
    static void onExccOffline(uint8_t index);

    // -----------------------------------------------------------------------
    // Booster (PROTO_07)
    // -----------------------------------------------------------------------
    static void onBooster(uint8_t index,
                          uint8_t etat,
                          uint8_t courant,
                          uint8_t tension,
                          uint8_t present);

    // Retourne l’index EXCC qui porte le booster (ou -1 si aucun)
    static int8_t getBoosterExccIndex();

    // -----------------------------------------------------------------------
    // Commandes CC → EXCC
    // -----------------------------------------------------------------------
    static void envoyerBoosterPower(uint8_t index, bool on); // F5
    static void demanderRecalibration(uint8_t index);        // F3
    static void envoyerSeuilsBooster(uint8_t index,
                                     uint16_t libre,
                                     uint16_t occupe);       // F4

private:
    // -----------------------------------------------------------------------
    // PING périodique (PROTO_PING)
    // -----------------------------------------------------------------------
    static void envoyerPingPeriodique();
};
