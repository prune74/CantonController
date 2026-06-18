#pragma once
#include <stdint.h>

/*
 * EXCC_Link.h — Communication RS485 CC ↔ EXCC
 * ---------------------------------------------------------------------------
 * Interface publique de la liaison série entre :
 *   - le Canton Controller (CC)
 *   - l’unique Extension Canton Controller (EXCC)
 *
 * Conception :
 *   → Un CC communique avec un seul EXCC
 *   → Le booster est intégré à l’EXCC
 *
 * Rôle :
 *   - supervision ONLINE / OFFLINE de l’EXCC
 *   - gestion du protocole UART (PING / PONG)
 *   - réception des informations du booster (état, tension, courant)
 *   - envoi des configurations (topologie, signaux, servos…)
 *   - envoi des seuils calibrés
 *   - commande ON/OFF du booster
 *
 * Ce module ne contient aucune logique métier :
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
    static bool isOnline();
    static void onPong();
    static void onExccOnline();
    static void onExccOffline();

    // -----------------------------------------------------------------------
    // Booster (PROTO_07)
    // -----------------------------------------------------------------------
    static void onBooster(uint8_t etat,
                          uint8_t courant,
                          uint8_t tension);

    // -----------------------------------------------------------------------
    // Commandes CC → EXCC
    // -----------------------------------------------------------------------
    static void envoyerBoosterPower(bool on);     // F5
    static void demanderRecalibration();          // F3
    static void envoyerSeuilsBooster(uint16_t libre,
                                     uint16_t occupe); // F4

private:
    // -----------------------------------------------------------------------
    // PING périodique (PROTO_PING)
    // -----------------------------------------------------------------------
    static void envoyerPingPeriodique();
};
