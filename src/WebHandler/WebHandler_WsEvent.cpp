/*
   WebHandler_WsEvent.cpp
   ------------------------------------------------------------
   Gestion des événements WebSocket pour le Signal Automate (SA).

   Ce fichier contient uniquement la logique liée aux événements
   WebSocket : connexion, déconnexion, erreurs, réception de données.

   Le rôle de ce fichier est de :
     - journaliser les événements WebSocket
     - notifier les clients lors d’une connexion
     - déléguer le traitement des données à handleWebSocketData()
     - garder WebHandler.cpp léger et lisible

   Toute la logique métier (aiguilles, settings, rôle, etc.) est
   traitée dans des fichiers spécialisés.
*/

#include "WebHandler.h"
#include "debug_sa.h"

// ---------------------------------------------------------------------------
// WsEvent()
// ---------------------------------------------------------------------------
// Fonction appelée automatiquement par la librairie AsyncWebSocket à chaque
// événement WebSocket.
//
// Paramètres :
//   - server : pointeur vers l'objet WebSocket
//   - client : client WebSocket concerné par l'événement
//   - type   : type d'événement (connexion, déconnexion, données, etc.)
//   - arg    : argument interne (non utilisé ici)
//   - data   : données reçues (si type == WS_EVT_DATA)
//   - len    : taille des données reçues
//
// Rôle :
//   - journaliser les événements WebSocket
//   - appeler notifyClients() lors d'une connexion
//   - déléguer le traitement des données à handleWebSocketData()
// ---------------------------------------------------------------------------
void WebHandler::WsEvent(AsyncWebSocket *server,
                         AsyncWebSocketClient *client,
                         AwsEventType type,
                         void *arg,
                         uint8_t *data,
                         size_t len)
{
    switch (type)
    {
        // ------------------------------------------------------------
        // Connexion d’un nouveau client WebSocket
        // ------------------------------------------------------------
        case WS_EVT_CONNECT:
            SA_LOG_INFO("[WebSocket] Client #%u connecté depuis %s\n",
                        client->id(),
                        client->remoteIP().toString().c_str());

            // On envoie immédiatement l’état complet du SA
            notifyClients();
            break;

        // ------------------------------------------------------------
        // Déconnexion d’un client WebSocket
        // ------------------------------------------------------------
        case WS_EVT_DISCONNECT:
            SA_LOG_INFO("[WebSocket] Client #%u déconnecté\n", client->id());
            break;

        // ------------------------------------------------------------
        // Erreur WebSocket
        // ------------------------------------------------------------
        case WS_EVT_ERROR:
            SA_LOG_WARN("[WebSocket] Erreur sur client #%u\n", client->id());
            break;

        // ------------------------------------------------------------
        // Réception d’un PONG (réponse au ping WebSocket)
        // ------------------------------------------------------------
        case WS_EVT_PONG:
            SA_LOG_TRACE("[WebSocket] PONG reçu du client #%u\n", client->id());
            break;

        // ------------------------------------------------------------
        // Réception de données WebSocket
        // ------------------------------------------------------------
        case WS_EVT_DATA:
            SA_LOG_TRACE("[WebSocket] Données reçues (%u octets)\n",
                         (unsigned)len);

            // On délègue le traitement du JSON à une fonction dédiée
            handleWebSocketData(client, data, len);
            break;
    }
}
