#pragma once

#include "Canton.h"
#include "CC_CAN.h"

/*
 * CommandeDCC.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Interface d’envoi des commandes DCC++ pour la locomotive du canton.
 *
 * Rôle :
 *   - récupérer la loco associée au canton
 *   - envoyer l’adresse + la vitesse via CC_CAN::sendMsg()
 *   - répéter l’envoi plusieurs fois pour fiabiliser la transmission
 *
 * Ce module ne contient aucune logique ferroviaire :
 *   → il transporte uniquement les commandes DCC++ vers la carte ERM.
 */

/*
 * envoyerCommande_DCC_LOCO_SPEED()
 * ---------------------------------------------------------------------------
 * Envoie la commande DCC++ correspondant à la locomotive du canton.
 *
 * Paramètre :
 *   - canton : pointeur vers le canton local
 *
 * Conditions :
 *   - si aucune loco n’est associée → aucun envoi
 *   - si la vitesse change → reset du compteur d’envoi
 */
void envoyerCommande_DCC_LOCO_SPEED(Canton *canton);

/*
 * envoyerCommande_DCC_WAGON_RAILCOM()
 * ---------------------------------------------------------------------------
 * Détournement de la commande DCC++ LOCO_FUNCTION pour activer/désactiver
 * RailCom sur un wagon donné.
 *
 * Pourquoi cette méthode ?
 *   - LaBox ne possède AUCUNE commande dédiée aux wagons.
 *   - LaBox diffuse le DCC globalement sur tout le réseau.
 *   - Les wagons reçoivent le DCC comme les locos (adresse DCC individuelle).
 *   - On détourne une fonction DCC (F27/F28) pour que le wagon interprète
 *     cette fonction comme "RailCom ON/OFF".
 *
 * Principe :
 *   - F27 = RailCom ON
 *   - F28 = RailCom OFF
 *
 *   LaBox envoie LOCO_FUNCTION(wagonAddr, Fxx=1)
 *   → Tous les décodeurs reçoivent la trame
 *   → Seul le wagon dont l’adresse DCC correspond exécute la commande
 *
 * Paramètres :
 *   - wagonAddr : adresse DCC du wagon (ex : 4013)
 *   - enable    : true  → activer RailCom (F27)
 *                 false → désactiver RailCom (F28)
 *
 * Remarque :
 *   - Cette fonction n’a AUCUNE logique ferroviaire.
 *   - Elle ne fait qu’envoyer une commande DCC++ vers LaBox via CC_CAN.
 *   - L’interprétation RailCom ON/OFF est faite dans le firmware du wagon.
 */
void envoyerCommande_DCC_WAGON_RAILCOM(uint16_t wagonAddr, bool enable);
