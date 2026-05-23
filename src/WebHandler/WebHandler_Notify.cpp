/*
   WebHandler_Notify.cpp — Discovery 2026 (CLEAN)
*/

#include "WebHandler.h"
#include "debug_sa.h"
#include "Node.h"
#include "Settings.h"
#include "Aig.h"
#include "Signal.h"

void WebHandler::notifyClients()
{
    StaticJsonDocument<1024> doc;

    // -----------------------------------------------------------------------
    // ID du nœud
    // -----------------------------------------------------------------------
    doc["idNode"] = node->ID();

    // -----------------------------------------------------------------------
    // Connexions P00/P01/P10/P11/M00/M01/M10/M11
    // -----------------------------------------------------------------------
    String index[] = {"p00", "p01", "p10", "p11",
                      "m00", "m01", "m10", "m11"};

    for (byte i = 0; i < 8; i++)
    {
        NodePeriph* p = node->getNodeP(i);

        if (!p)
            doc[index[i]] = "null";
        else
            doc[index[i]] = p->ID();
    }

    // -----------------------------------------------------------------------
    // Aiguilles
    // -----------------------------------------------------------------------
    for (byte i = 0; i < aigSize; i++)
    {
        Aig* a = node->getAig(i);

        if (!a)
        {
            doc["s" + String(i)]       = "null";
            doc["s" + String(i) + "0"] = "";
            doc["s" + String(i) + "1"] = "";
            doc["s" + String(i) + "2"] = "";   // vitesse supprimée
        }
        else
        {
            doc["s" + String(i)]       = "Actif";
            doc["s" + String(i) + "0"] = a->posDroit();
            doc["s" + String(i) + "1"] = a->posDevie();

            // -------------------------------------------------------------------
            // La vitesse n'est plus dans Aig.
            // On renvoie une valeur neutre (ou issue du JSON si tu veux).
            // -------------------------------------------------------------------
            doc["s" + String(i) + "2"] = 0;   // vitesse supprimée en 2026
        }
    }

    // -----------------------------------------------------------------------
    // Paramètres système
    // -----------------------------------------------------------------------
    doc["wifi_on"]      = Settings::wifiOn();
    doc["discovery_on"] = Settings::discoveryOn();
    doc["maxSpeed"]     = node->maxSpeed();
    doc["sensMarche"]   = node->sensMarche();

    // -----------------------------------------------------------------------
    // Rôle ferroviaire
    // -----------------------------------------------------------------------
    doc["role"] = static_cast<uint8_t>(node->getRole());

    // -----------------------------------------------------------------------
    // Signaux (cible horaire / antihoraire)
    // -----------------------------------------------------------------------
    Signal* sigAH = node->getSignal(0);
    Signal* sigH  = node->getSignal(1);

    if (sigH)
        doc["cibleHoraire"] = sigH->type();

    if (sigAH)
        doc["cibleAntiHor"] = sigAH->type();

    // -----------------------------------------------------------------------
    // Sérialisation et envoi
    // -----------------------------------------------------------------------
    String output;
    serializeJson(doc, output);

    _ws->textAll(output);

    SA_LOG_TRACE("[Notify] État complet envoyé aux clients WebSocket\n");
}
