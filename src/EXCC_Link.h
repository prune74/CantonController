#pragma once
#include <stdint.h>

/*
 * EXCC_Link.h — Communication CAN CC ↔ EXCC
 * ---------------------------------------------------------------------------
 * Interface publique de la liaison logique entre :
 *   - le Canton Controller (CC)
 *   - l’unique Extension Canton Controller (EXCC)
 *
 * Conception :
 *   → Un CC communique avec un seul EXCC (associé à un Canton unique)
 *   → Le booster est intégré à l’EXCC
 *
 * Rôle :
 *   - supervision ONLINE / OFFLINE de l’EXCC
 *   - gestion du protocole CAN (PING / PONG, commandes 0xD0–0xDF)
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
    static void onPong();
    static void onExccOnline();
    static void onExccOffline();

    // -----------------------------------------------------------------------
    // Booster (info reçue depuis l’EXCC)
    // -----------------------------------------------------------------------
    static void onBooster(uint8_t etat,
                          uint8_t courant,
                          uint8_t tension);

    // -----------------------------------------------------------------------
    // Commandes CC → EXCC (via CAN)
    // -----------------------------------------------------------------------
    static void envoyerBoosterPower(bool on); // cmd CMD_CC_EXCC_BOOSTER_POWER
    static void demanderRecalibration();      // cmd CMD_EXCC_CC_CALIB_BOOSTER_INFO
    static void envoyerSeuilsBooster(uint16_t libre,
                                     uint16_t occupe); // cmd CMD_CC_EXCC_SET_SEUILS

private:
    // -----------------------------------------------------------------------
    // PING périodique (commande CAN CMD_CC_EXCC_PING)
    // -----------------------------------------------------------------------
    static void envoyerPingPeriodique();
};
