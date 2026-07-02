/*
 * Settings_JSON_Direction.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Gestion des feux directionnels H / AH dans settings.json.
 *
 * Format JSON attendu :
 *
 *   "direction": {
 *      "H": {
 *         "active": true,
 *         "codeBarre": "A12",
 *         "voieDuVoisin": { "12": 1, "14": 2 }
 *      },
 *      "AH": {
 *         "active": false,
 *         "codeBarre": "",
 *         "voieDuVoisin": {}
 *      }
 *   }
 *
 * Ce module ne contient aucune logique métier :
 *   → il synchronise simplement les données JSON ↔ structures Canton.
 */

#include "Settings.h"
#include "Canton.h"
#include "debug_cc.h"
#include <ArduinoJson.h>

/* ============================================================================
 *  Chargement des feux directionnels
 * ==========================================================================*/
void Settings_JSON_loadDirection(Canton *canton, JsonDocument &doc)
{
    JsonVariant vDir = doc["direction"];
    if (vDir.isNull())
        return;

    JsonObject dir = vDir.as<JsonObject>();

    // -----------------------------------------------------------------------
    // Direction H
    // -----------------------------------------------------------------------
    {
        JsonVariant vH = dir["H"];
        if (!vH.isNull())
        {
            JsonObject h = vH.as<JsonObject>();

            canton->directionH().active    = h["active"]    | false;
            canton->directionH().codeBarre = h["codeBarre"] | "";

            JsonVariant vMap = h["voieDuVoisin"];
            if (!vMap.isNull())
            {
                JsonObject map = vMap.as<JsonObject>();
                for (JsonPair kv : map)
                {
                    uint16_t id  = atoi(kv.key().c_str());
                    uint8_t voie = kv.value() | 0;
                    canton->directionH().voieDuVoisin[id] = voie;
                }
            }
        }
    }

    // -----------------------------------------------------------------------
    // Direction AH
    // -----------------------------------------------------------------------
    {
        JsonVariant vAH = dir["AH"];
        if (!vAH.isNull())
        {
            JsonObject ah = vAH.as<JsonObject>();

            canton->directionAH().active    = ah["active"]    | false;
            canton->directionAH().codeBarre = ah["codeBarre"] | "";

            JsonVariant vMap = ah["voieDuVoisin"];
            if (!vMap.isNull())
            {
                JsonObject map = vMap.as<JsonObject>();
                for (JsonPair kv : map)
                {
                    uint16_t id  = atoi(kv.key().c_str());
                    uint8_t voie = kv.value() | 0;
                    canton->directionAH().voieDuVoisin[id] = voie;
                }
            }
        }
    }

    CC_LOG_INFO("[Settings][Direction][CC] Feux directionnels chargés\n");
}

/* ============================================================================
 *  Sauvegarde des feux directionnels
 * ==========================================================================*/
void Settings_JSON_saveDirection(Canton *canton, JsonDocument &doc)
{
    JsonObject dir = doc.createNestedObject("direction");

    // -----------------------------------------------------------------------
    // Direction H
    // -----------------------------------------------------------------------
    {
        JsonObject h = dir.createNestedObject("H");
        h["active"]    = canton->directionH().active;
        h["codeBarre"] = canton->directionH().codeBarre;

        JsonObject map = h.createNestedObject("voieDuVoisin");
        for (auto &kv : canton->directionH().voieDuVoisin)
            map[String(kv.first)] = kv.second;
    }

    // -----------------------------------------------------------------------
    // Direction AH
    // -----------------------------------------------------------------------
    {
        JsonObject ah = dir.createNestedObject("AH");
        ah["active"]    = canton->directionAH().active;
        ah["codeBarre"] = canton->directionAH().codeBarre;

        JsonObject map = ah.createNestedObject("voieDuVoisin");
        for (auto &kv : canton->directionAH().voieDuVoisin)
            map[String(kv.first)] = kv.second;
    }
}
