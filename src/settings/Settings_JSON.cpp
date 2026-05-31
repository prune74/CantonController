/*
   Settings_JSON.cpp — Discovery 2026 (CLEAN & FIXED, MINIMAL)
*/

#include "Settings.h"
#include "Node.h"
#include "Aig.h"
#include "debug_sa.h"
#include <ArduinoJson.h>
#include <SPIFFS.h>

/* ============================================================================
   Document JSON global en RAM
   ==========================================================================*/
static StaticJsonDocument<4096> settingsDoc;

/* ============================================================================
   Booster — Seuils calibrés (PROTO_09)
   ==========================================================================*/
uint16_t Settings::s_boosterSeuilLibre  = 0;
uint16_t Settings::s_boosterSeuilOccupe = 0;

/* ============================================================================
   API JSON (set/get/save/load)
   ==========================================================================*/

void Settings::load()
{
    if (!SPIFFS.begin(true))
        return;

    File file = SPIFFS.open("/settings.json", "r");
    if (!file)
        return;

    DeserializationError err = deserializeJson(settingsDoc, file);
    file.close();

    if (err)
    {
        SA_LOG_ERROR("[Settings] Erreur JSON (load): %s\n", err.c_str());
        return;
    }

    // Charger les seuils booster
    s_boosterSeuilLibre  = settingsDoc["booster_seuil_libre"]  | 0;
    s_boosterSeuilOccupe = settingsDoc["booster_seuil_occupe"] | 0;

    SA_LOG_INFO("[Settings] Seuils booster chargés → libre=%u occupe=%u\n",
                s_boosterSeuilLibre, s_boosterSeuilOccupe);
}

void Settings::save()
{
    if (!SPIFFS.begin(true))
        return;

    // Sauvegarder les seuils booster
    settingsDoc["booster_seuil_libre"]  = s_boosterSeuilLibre;
    settingsDoc["booster_seuil_occupe"] = s_boosterSeuilOccupe;

    File file = SPIFFS.open("/settings.json", "w");
    if (!file)
        return;

    serializeJsonPretty(settingsDoc, file);
    file.close();

    SA_LOG_INFO("[Settings] settings.json sauvegardé\n");
}

uint16_t Settings::get(const char* key)
{
    return settingsDoc[key] | 0;
}

void Settings::set(const char* key, uint16_t value)
{
    settingsDoc[key] = value;
}

/* ============================================================================
   Chargement des aiguilles depuis un JsonDocument
   ==========================================================================*/
static void loadAiguilles(Node* node, JsonDocument& doc)
{
    for (byte i = 0; i < aigSize; i++)
    {
        Aig* a = node->getAig(i);
        if (!a)
            continue;

        String key = "aig" + String(i);

        if (doc.containsKey(key + "posDroit"))
            a->posDroit(doc[key + "posDroit"]);

        if (doc.containsKey(key + "posDevie"))
            a->posDevie(doc[key + "posDevie"]);
    }
}

/* ============================================================================
   Sauvegarde des aiguilles dans un JsonDocument
   ==========================================================================*/
static void saveAiguilles(Node* node, JsonDocument& doc)
{
    for (byte i = 0; i < aigSize; i++)
    {
        Aig* a = node->getAig(i);
        if (!a)
            continue;

        String key = "aig" + String(i);

        doc[key + "posDroit"] = a->posDroit();
        doc[key + "posDevie"] = a->posDevie();
    }
}

/* ============================================================================
   Chargement FeuxDirection (H / AH)
   ==========================================================================*/
static void loadDirection(Node* node, JsonDocument& doc)
{
    if (!doc.containsKey("direction"))
        return;

    JsonObject dir = doc["direction"];

    // --- H ---
    if (dir.containsKey("H"))
    {
        JsonObject h = dir["H"];
        node->directionH().active = h["active"] | false;
        node->directionH().codeBarre = h["codeBarre"] | "";

        if (h.containsKey("voieDuVoisin"))
        {
            JsonObject map = h["voieDuVoisin"];
            for (JsonPair kv : map)
            {
                uint16_t id = atoi(kv.key().c_str());
                uint8_t voie = kv.value() | 0;
                node->directionH().voieDuVoisin[id] = voie;
            }
        }
    }

    // --- AH ---
    if (dir.containsKey("AH"))
    {
        JsonObject ah = dir["AH"];
        node->directionAH().active = ah["active"] | false;
        node->directionAH().codeBarre = ah["codeBarre"] | "";

        if (ah.containsKey("voieDuVoisin"))
        {
            JsonObject map = ah["voieDuVoisin"];
            for (JsonPair kv : map)
            {
                uint16_t id = atoi(kv.key().c_str());
                uint8_t voie = kv.value() | 0;
                node->directionAH().voieDuVoisin[id] = voie;
            }
        }
    }
}

/* ============================================================================
   Sauvegarde FeuxDirection (H / AH)
   ==========================================================================*/
static void saveDirection(Node* node, JsonDocument& doc)
{
    JsonObject dir = doc.createNestedObject("direction");

    // --- H ---
    {
        JsonObject h = dir.createNestedObject("H");
        h["active"] = node->directionH().active;
        h["codeBarre"] = node->directionH().codeBarre;

        JsonObject map = h.createNestedObject("voieDuVoisin");
        for (auto& kv : node->directionH().voieDuVoisin)
            map[String(kv.first)] = kv.second;
    }

    // --- AH ---
    {
        JsonObject ah = dir.createNestedObject("AH");
        ah["active"] = node->directionAH().active;
        ah["codeBarre"] = node->directionAH().codeBarre;

        JsonObject map = ah.createNestedObject("voieDuVoisin");
        for (auto& kv : node->directionAH().voieDuVoisin)
            map[String(kv.first)] = kv.second;
    }
}

/* ============================================================================
   Chargement global settings.json → Node + settingsDoc
   ==========================================================================*/
void Settings::loadFile(Node* node)
{
    if (!SPIFFS.begin(true))
    {
        SA_LOG_ERROR("[Settings] SPIFFS indisponible\n");
        return;
    }

    File file = SPIFFS.open("/settings.json", "r");
    if (!file)
    {
        SA_LOG_WARN("[Settings] settings.json introuvable\n");
        return;
    }

    StaticJsonDocument<4096> doc;
    DeserializationError err = deserializeJson(doc, file);
    file.close();

    if (err)
    {
        SA_LOG_ERROR("[Settings] Erreur JSON (loadFile): %s\n", err.c_str());
        return;
    }

    loadAiguilles(node, doc);

    if (doc.containsKey("maxSpeed"))
        node->maxSpeed(doc["maxSpeed"]);

    // FeuxDirection
    loadDirection(node, doc);

    // Booster
    s_boosterSeuilLibre  = doc["booster_seuil_libre"]  | 0;
    s_boosterSeuilOccupe = doc["booster_seuil_occupe"] | 0;

    settingsDoc = doc;

    SA_LOG_INFO("[Settings] settings.json chargé\n");
}

/* ============================================================================
   Sauvegarde globale Node → settings.json
   ==========================================================================*/
void Settings::writeFile(Node* node)
{
    if (!SPIFFS.begin(true))
    {
        SA_LOG_ERROR("[Settings] SPIFFS indisponible\n");
        return;
    }

    StaticJsonDocument<4096> doc = settingsDoc;

    saveAiguilles(node, doc);

    doc["maxSpeed"] = node->maxSpeed();

    // FeuxDirection
    saveDirection(node, doc);

    // Booster
    doc["booster_seuil_libre"]  = s_boosterSeuilLibre;
    doc["booster_seuil_occupe"] = s_boosterSeuilOccupe;

    settingsDoc = doc;

    File file = SPIFFS.open("/settings.json", "w");
    if (!file)
    {
        SA_LOG_ERROR("[Settings] Impossible d’écrire settings.json\n");
        return;
    }

    serializeJsonPretty(doc, file);
    file.close();

    SA_LOG_INFO("[Settings] settings.json sauvegardé\n");
}

/* ============================================================================
   Booster — Getters / Setters
   ==========================================================================*/
void Settings::setBoosterSeuilLibre(uint16_t v)
{
    s_boosterSeuilLibre = v;
}

void Settings::setBoosterSeuilOccupe(uint16_t v)
{
    s_boosterSeuilOccupe = v;
}

uint16_t Settings::boosterSeuilLibre()
{
    return s_boosterSeuilLibre;
}

uint16_t Settings::boosterSeuilOccupe()
{
    return s_boosterSeuilOccupe;
}

/* ------------------------------------------------------------
  Fin de Settings_JSON.cpp
  ------------------------------------------------------------
*/
