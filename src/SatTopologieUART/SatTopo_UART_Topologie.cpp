/*
 * SatTopo_UART_Topologie.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Transmission de la topologie CAN (voisins SP/SM) vers les EXCC via UART.
 *
 * OpCode :
 *   - E4 : topologie CAN (idLocal + précédents + suivants)
 *
 * Rôle :
 *   - lire settings.json
 *   - construire les listes de précédents / suivants
 *   - envoyer la trame E4 aux EXCC
 *   - déclencher l’envoi une seule fois lorsque la topologie est prête
 *
 * IMPORTANT :
 *   - aucune logique métier ici
 *   - aucune déduction topologique
 *   - aucune interprétation ferroviaire
 *
 * Ce module transmet simplement à l’EXCC ce que settings.json contient.
 */

#include "SatTopologieUART.h"
#include "Config.h"
#include "Exploration_Protocol.h"
#include "debug_cc.h"

#include <SPIFFS.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <vector>

extern HardwareSerial Serial1;

/* ============================================================================
 *  Vérifie si au moins un voisin est connu dans settings.json
 * ==========================================================================*/
static bool tousLesVoisinsSontConnus()
{
    CC_LOG_TRACE("[TopoUART][CC] Vérification des voisins dans settings.json\n");

    if (!SPIFFS.begin(true))
    {
        CC_LOG_ERROR("[TopoUART][CC] SPIFFS indisponible\n");
        return false;
    }

    File file = SPIFFS.open("/settings.json", "r");
    if (!file)
    {
        CC_LOG_WARN("[TopoUART][CC] settings.json introuvable\n");
        return false;
    }

    StaticJsonDocument<4096> doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error)
    {
        CC_LOG_ERROR("[TopoUART][CC] Erreur JSON : %s\n", error.c_str());
        return false;
    }

    uint8_t compteur = 0;
    for (const char *cle : {"p00", "p01", "p10", "p11",
                            "m00", "m01", "m10", "m11"})
    {
        if (doc[cle].is<uint8_t>() && doc[cle] != UNUSED_ID)
            compteur++;
    }

    CC_LOG_INFO("[TopoUART][CC] %u voisins connus\n", compteur);

    return compteur >= 1;
}

/* ============================================================================
 *  Envoie la topologie CAN depuis settings.json — opcode E4
 * ==========================================================================*/
void envoyerTopologieDepuisSettings()
{
    CC_LOG_INFO("[TopoUART][CC] Envoi de la topologie EXCC...\n");

    if (!SPIFFS.begin(true))
    {
        CC_LOG_ERROR("[TopoUART][CC] SPIFFS indisponible\n");
        return;
    }

    File file = SPIFFS.open("/settings.json", "r");
    if (!file)
    {
        CC_LOG_ERROR("[TopoUART][CC] settings.json introuvable\n");
        return;
    }

    StaticJsonDocument<4095> doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error)
    {
        CC_LOG_ERROR("[TopoUART][CC] Erreur JSON : %s\n", error.c_str());
        return;
    }

    uint8_t idLocal = doc["idCanton"] | UNUSED_ID;

    // -----------------------------------------------------------------------
    // Construction des listes précédents / suivants
    // -----------------------------------------------------------------------
    std::vector<uint8_t> precedents;
    for (const char *cle : {"m00", "m01", "m10", "m11"})
    {
        if (doc[cle].is<uint8_t>())
        {
            uint8_t id = doc[cle];
            if (id != UNUSED_ID)
                precedents.push_back(id);
        }
    }

    std::vector<uint8_t> suivants;
    for (const char *cle : {"p00", "p01", "p10", "p11"})
    {
        if (doc[cle].is<uint8_t>())
        {
            uint8_t id = doc[cle];
            if (id != UNUSED_ID)
                suivants.push_back(id);
        }
    }

    CC_LOG_TRACE("[TopoUART][CC] idLocal=%u, precedents=%u, suivants=%u\n",
                 idLocal, precedents.size(), suivants.size());

    // -----------------------------------------------------------------------
    // Envoi E4
    // -----------------------------------------------------------------------
    Serial1.write(PROTO_SYNC_BYTE);
    Serial1.write(PROTO_E4_TOPOLOGIE_CAN);
    Serial1.write(idLocal);

    Serial1.write(precedents.size());
    for (uint8_t id : precedents)
        Serial1.write(id);

    Serial1.write(suivants.size());
    for (uint8_t id : suivants)
        Serial1.write(id);

    CC_LOG_INFO("[TopoUART][CC] Topologie envoyée à EXCC\n");
}

/* ============================================================================
 *  Envoi conditionnel de la topologie (une seule fois)
 * ==========================================================================*/
void envoyerTopologieSiPret()
{
    static bool dejaEnvoyee = false;

    if (dejaEnvoyee)
        return;

    if (tousLesVoisinsSontConnus())
    {
        CC_LOG_INFO("[TopoUART][CC] Topologie prête → envoi\n");

        envoyerTopologieDepuisSettings();
        envoyerConfigurationSignauxDepuisSettings();

        dejaEnvoyee = true;
    }
    else
    {
        CC_LOG_TRACE("[TopoUART][CC] Topologie non prête\n");
    }
}
