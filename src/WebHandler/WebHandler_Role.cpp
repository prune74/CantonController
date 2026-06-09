/*
   WebHandler_Role.cpp — Exploration 2026 (FINAL & CLEAN)
*/

#include "WebHandler.h"
#include "debug_sa.h"
#include "Settings.h"

// ---------------------------------------------------------------------------
// handleRole()
// ---------------------------------------------------------------------------
// JSON attendu :
//   { "cmd": "setRole", "value": 3 }
//
// Rôle :
//   - mettre à jour canton->setRole()
//   - sauvegarder dans settings.json
//   - notifier les clients WebSocket
// ---------------------------------------------------------------------------
void WebHandler::handleRole(JsonDocument &doc)
{
    if (!doc.containsKey("value"))
    {
        SA_LOG_WARN("[Role] Commande setRole sans 'value'\n");
        return;
    }

    uint8_t role = doc["value"];

    SA_LOG_INFO("[Role] Nouveau rôle demandé : %u\n", role);

    // Mise à jour logique interne
    canton->setRole((CantonRole)role);

    // Sauvegarde dans settings.json
    Settings::set("role", role);
    Settings::save();

    SA_LOG_INFO("[Role] Rôle mis à jour et sauvegardé\n");

    // Notifier l’interface Web
    StaticJsonDocument<64> out;
    out["cmd"] = "roleUpdate";
    out["role"] = role;

    String json;
    serializeJson(out, json);
    _ws->textAll(json);

    // Mise à jour complète
    notifyClients();
}
