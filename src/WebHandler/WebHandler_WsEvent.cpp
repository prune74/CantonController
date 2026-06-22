/*
 * WebHandler_WsEvent.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Gestion des événements WebSocket :
 *   - connexion / déconnexion
 *   - erreurs
 *   - réception de données
 *   - PONG keep‑alive
 *
 * Ce module ne contient aucune logique métier :
 *   → il délègue le traitement JSON à handleWebSocketData().
 */

#include "WebHandler.h"
#include "debug_cc.h"

// ---------------------------------------------------------------------------
// WsEvent()
// ---------------------------------------------------------------------------
void WebHandler::WsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) // 🟢
{
    switch (type)
    {
        // ------------------------------------------------------------
        // Connexion d’un client WebSocket
        // ------------------------------------------------------------
        case WS_EVT_CONNECT:
            CC_LOG_INFO("[WebSocket][CC] Client #%u connecté depuis %s\n",
                        client->id(),
                        client->remoteIP().toString().c_str());

            // Envoi immédiat de l’état complet du CC
            notifyClients();
            break;

        // ------------------------------------------------------------
        // Déconnexion d’un client WebSocket
        // ------------------------------------------------------------
        case WS_EVT_DISCONNECT:
            CC_LOG_INFO("[WebSocket][CC] Client #%u déconnecté\n",
                        client->id());
            break;

        // ------------------------------------------------------------
        // Erreur WebSocket
        // ------------------------------------------------------------
        case WS_EVT_ERROR:
            CC_LOG_WARN("[WebSocket][CC] Erreur sur client #%u\n",
                        client->id());
            break;

        // ------------------------------------------------------------
        // Réception d’un PONG
        // ------------------------------------------------------------
        case WS_EVT_PONG:
            CC_LOG_TRACE("[WebSocket][CC] PONG reçu du client #%u\n",
                         client->id());
            break;

        // ------------------------------------------------------------
        // Réception de données WebSocket
        // ------------------------------------------------------------
        case WS_EVT_DATA:
            CC_LOG_TRACE("[WebSocket][CC] Données reçues (%u octets)\n",
                         (unsigned)len);

            // Délégation au parseur JSON
            handleWebSocketData(client, data, len);
            break;
    }
}
