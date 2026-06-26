/*
 * Settings_JSON_Voisins.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Chargement / sauvegarde des voisins ferroviaires du canton.
 *
 * Clés JSON :
 *   p00, p01, p10, p11
 *   m00, m01, m10, m11
 *
 * Ce module ne contient aucune logique métier :
 *   → il synchronise simplement JSON ↔ structures CantonPeriph.
 */

#include "Settings.h"
#include "Canton.h"
#include "debug_cc.h"
#include <ArduinoJson.h>

/* ============================================================================
 *  Table de correspondance index → clé JSON
 * ==========================================================================*/
static const char *VOISIN_KEY[cantonPsize] =
    {
        "p00", "p01", "p10", "p11",
        "m00", "m01", "m10", "m11"};

/* ============================================================================
 *  Chargement des voisins
 * ==========================================================================*/
void Settings_JSON_loadVoisins(Canton *canton, JsonDocument &doc) // 🟢
{
    for (uint8_t i = 0; i < cantonPsize; i++)
    {
        const char *key = VOISIN_KEY[i];

        // -------------------------------------------------------------------
        // Clé absente → aucun voisin
        // -------------------------------------------------------------------
        if (!doc.containsKey(key))
        {
            canton->setCantonP(i, nullptr);
            continue;
        }

        // -------------------------------------------------------------------
        // Valeur "null" → aucun voisin
        // -------------------------------------------------------------------
        if (doc[key].is<const char *>() && strcmp(doc[key], "null") == 0)
        {
            canton->setCantonP(i, nullptr);
            continue;
        }

        // -------------------------------------------------------------------
        // ID du voisin
        // -------------------------------------------------------------------
        uint8_t id = doc[key] | UNUSED_ID;

        if (id == UNUSED_ID)
        {
            canton->setCantonP(i, nullptr);
            continue;
        }

        // -------------------------------------------------------------------
        // Création si nécessaire
        // -------------------------------------------------------------------
        CantonPeriph *p = canton->getCantonP(i);
        if (!p)
        {
            p = new CantonPeriph;
            canton->setCantonP(i, p);
        }

        p->ID(id);
    }

    CC_LOG_INFO("[Settings][Voisins][CC] Voisins chargés\n");
}

/* ============================================================================
 *  Sauvegarde des voisins
 * ==========================================================================*/
void Settings_JSON_saveVoisins(Canton *canton, JsonDocument &doc) // 🟢
{
    for (uint8_t i = 0; i < cantonPsize; i++)
    {
        const char *key = VOISIN_KEY[i];
        CantonPeriph *p = canton->getCantonP(i);

        if (!p)
        {
            doc[key] = "null";
        }
        else
        {
            doc[key] = p->ID();
        }
    }
}
