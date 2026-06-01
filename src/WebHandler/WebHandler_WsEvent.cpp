/*
   WebHandler_WsEvent.cpp — Discovery 2026 (FINAL & CLEAN)
*/

#include "WebHandler.h"
#include "debug_sa.h"

// ---------------------------------------------------------------------------
// WsEvent()
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
        // Connexion d’un client WebSocket
        // ------------------------------------------------------------
        case WS_EVT_CONNECT:
            SA_LOG_INFO("[WebSocket] Client #%u connecté depuis %s\n",
                        client->id(),
                        client->remoteIP().toString().c_str());

            // Envoi immédiat de l’état complet du SA
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
        // Réception d’un PONG
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

            // Délégation au parseur JSON
            handleWebSocketData(client, data, len);
            break;
    }
}
