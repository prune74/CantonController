/*
 * WebHandler_Role.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Gestion du rôle ferroviaire du canton via WebSocket.
 *
 * JSON attendu :
 *   { "cmd": "setRole", "value": <role> }
 *
 * Rôle :
 *   - mettre à jour canton->setRole()
 *   - sauvegarder dans settings.json (source de vérité 2026)
 *   - notifier l’interface Web (roleUpdate)
 *   - renvoyer l’état complet (notifyClients)
 */

#include "WebHandler.h"
#include "debug_cc.h"
#include "Settings.h"

// ---------------------------------------------------------------------------
// handleRole()
// ---------------------------------------------------------------------------
void WebHandler::handleRole(JsonDocument &doc)
{
    if (!doc.containsKey("value"))
    {
        CC_LOG_WARN("[Role][CC] Commande setRole sans 'value'\n");
        return;
    }

    uint8_t role = doc["value"];

    CC_LOG_INFO("[Role][CC] Nouveau rôle demandé : %u\n", role);

    // -----------------------------------------------------------------------
    // 1) Mise à jour logique interne
    // -----------------------------------------------------------------------
    canton->setRole(static_cast<CantonRole>(role));

    // -----------------------------------------------------------------------
    // 2) Sauvegarde JSON 2026
    // -----------------------------------------------------------------------
    Settings::writeFile(canton);

    CC_LOG_INFO("[Role][CC] Rôle mis à jour et sauvegardé\n");

    // -----------------------------------------------------------------------
    // 3) Notification WebSocket (événement roleUpdate)
    // -----------------------------------------------------------------------
    StaticJsonDocument<64> out;
    out["cmd"]  = "roleUpdate";
    out["role"] = role;

    String json;
    serializeJson(out, json);
    _ws->textAll(json);

    // -----------------------------------------------------------------------
    // 4) Mise à jour complète de l’interface
    // -----------------------------------------------------------------------
    notifyClients();
}
