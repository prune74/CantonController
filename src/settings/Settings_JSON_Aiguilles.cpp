/*
 * Settings_JSON_Aiguilles.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Chargement / sauvegarde des AIGUILLES LOGIQUES depuis/vers settings.json.
 *
 * Rôle :
 *   - créer ou supprimer les objets Aig selon le JSON
 *   - charger les paramètres logiques :
 *        • ID
 *        • posDroit / posDevie
 *        • index EXCC côté H / AH
 *        • vitesse (stockée dans servoCfg, plus dans Aig)
 *   - sauvegarder ces mêmes paramètres dans settings.json
 *
 * Ce module ne contient aucune logique métier :
 *   → il synchronise simplement l’état logique avec le JSON.
 */

#include "Settings.h"
#include "Canton.h"
#include "Aig.h"
#include "debug_cc.h"
#include <ArduinoJson.h>

/* ============================================================================
 *  Chargement des aiguilles LOGIQUES
 * ==========================================================================*/
void Settings_JSON_loadAiguilles(Canton *canton, JsonDocument &doc)
{
    // -------------------------------------------------------------------
    // Chargement du masque interne des aiguilles
    // -------------------------------------------------------------------
    canton->masqueAigInterne(doc["masqueAigInterne"] | 0);

    // -------------------------------------------------------------------
    // Chargement des aiguilles logiques
    // -------------------------------------------------------------------
    for (uint8_t i = 0; i < aigSize; i++)
    {
        String base = "aig" + String(i);

        // -------------------------------------------------------------------
        // Aiguille absente → nullptr
        // -------------------------------------------------------------------
        JsonVariant v = doc[base];

        if (v.isNull() ||
            (v.is<const char *>() && strcmp(v.as<const char *>(), "null") == 0))
        {
            canton->setAig(i, nullptr);
            continue;
        }

        // -------------------------------------------------------------------
        // Création si nécessaire
        // -------------------------------------------------------------------
        Aig *a = canton->getAig(i);
        if (!a)
        {
            a = new Aig;
            canton->setAig(i, a);
        }

        // -------------------------------------------------------------------
        // Chargement des champs logiques
        // -------------------------------------------------------------------
        JsonObject obj = doc[base].as<JsonObject>();

        a->ID(obj["id"] | 0);
        a->posDroit(obj["posDroit"] | 1500);
        a->posDevie(obj["posDevie"] | 1500);
        canton->getServoCfg(i).speed = obj["speed"] | 0;
    }

    // -------------------------------------------------------------------
    // Sauvegarde du masque interne des aiguilles
    // -------------------------------------------------------------------
    doc["masqueAigInterne"] = canton->masqueAigInterne();

    CC_LOG_INFO("[Settings][Aiguilles][CC] Aiguilles logiques chargées (speed via servoCfg)\n");
}

/* ============================================================================
 *  Sauvegarde des aiguilles LOGIQUES
 * ==========================================================================*/
void Settings_JSON_saveAiguilles(Canton *canton, JsonDocument &doc)
{
    for (uint8_t i = 0; i < aigSize; i++)
    {
        String base = "aig" + String(i);
        Aig *a = canton->getAig(i);

        // -------------------------------------------------------------------
        // Aiguille absente
        // -------------------------------------------------------------------
        if (!a)
        {
            doc[base] = "null";
            continue;
        }

        // -------------------------------------------------------------------
        // Aiguille active
        // -------------------------------------------------------------------
        JsonObject obj = doc[base].to<JsonObject>();

        obj["state"] = "Actif";
        obj["id"] = a->ID();
        obj["posDroit"] = a->posDroit();
        obj["posDevie"] = a->posDevie();
        obj["speed"] = canton->getServoCfg(i).speed;
    }
}
