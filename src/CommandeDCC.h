#pragma once

#include "Canton.h"
#include "CanMsg.h"

/*
 * CommandeDCC.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Interface d’envoi des commandes DCC++ pour la locomotive du canton.
 *
 * Rôle :
 *   - récupérer la loco associée au canton
 *   - envoyer l’adresse + la vitesse via CanMsg::sendMsg()
 *   - répéter l’envoi plusieurs fois pour fiabiliser la transmission
 *
 * Ce module ne contient aucune logique ferroviaire :
 *   → il transporte uniquement les commandes DCC++ vers la carte Main.
 */

/*
 * envoyerCommandeDCC()
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
void envoyerCommandeDCC(Canton *canton);
