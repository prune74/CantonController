/*
   WebHandler_Role.cpp
   ------------------------------------------------------------
   Gestion de la commande WebSocket "setRole" permettant de définir
   le rôle ferroviaire du canton (ex : NORMAL, TERMINUS, AIGUILLE…).

   Le rôle influence la logique métier du SA :
     - comportement des signaux
     - gestion des aiguilles
     - règles de sécurité
     - propagation des états aux nœuds voisins

   Ce fichier isole cette logique pour garder WebHandler lisible
   et faciliter l’onboarding des futurs contributeurs.
*/

#include "WebHandler.h"
#include "debug_sa.h"
#include "Settings.h"

// ---------------------------------------------------------------------------
// handleRole()
// ---------------------------------------------------------------------------
// Traite la commande Web "setRole" envoyée par l’interface Web.
//
// Exemple de trame JSON reçue :
//   { "setRole": 2 }
//
// Rôle :
//   - convertir la valeur reçue en enum CantonRole
//   - mettre à jour la logique interne du SA
//   - sauvegarder settings.json
//   - notifier les clients WebSocket pour mise à jour immédiate
//
// Cette commande est critique car elle modifie le comportement
// ferroviaire du canton.
// ---------------------------------------------------------------------------
void WebHandler::handleRole(JsonDocument &doc)
{
    // Récupération du rôle demandé
    uint8_t role = doc["setRole"];

    SA_LOG_INFO("[Role] Nouveau rôle demandé : %u\n", role);

    // Mise à jour de la logique interne
    node->setRole((CantonRole)role);

    // Sauvegarde dans settings.json
    Settings::writeFile(Settings::node);

    SA_LOG_INFO("[Role] Rôle mis à jour et sauvegardé\n");

    // Mise à jour immédiate de l’interface Web
    notifyClients();
}
