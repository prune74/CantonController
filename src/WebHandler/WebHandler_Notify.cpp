/*
 * WebHandler_Notify.cpp — Publication de l’état CC → WebSocket
 * ---------------------------------------------------------------------------
 * Ce module construit un JSON compact représentant l’état complet du canton :
 *   - ID, topologie, aiguilles, signaux
 *   - paramètres système (wifi, exploration, maxSpeed…)
 *   - mesures Booster + seuils
 *
 * Il ne contient aucune logique métier :
 *   → il expose simplement l’état interne du CC.
 */

#include "WebHandler.h"
#include "debug_cc.h"
#include "Canton.h"
#include "Settings.h"
#include "Aig.h"
#include "Signal.h"
#include "Booster.h"

void WebHandler::notifyClients()
{
    if (!_ws || _ws->count() == 0)
        return;

    StaticJsonDocument<1024> doc;

    // -----------------------------------------------------------------------
    // ID du canton
    // -----------------------------------------------------------------------
    doc["idCanton"] = canton->ID();

    // -----------------------------------------------------------------------
    // Connexions P00/P01/P10/P11/M00/M01/M10/M11
    // -----------------------------------------------------------------------
    const char *index[] = {"p00", "p01", "p10", "p11",
                           "m00", "m01", "m10", "m11"};

    for (uint8_t i = 0; i < 8; i++)
    {
        CantonPeriph *p = canton->getCantonP(i);
        if (p)
            doc[index[i]] = p->ID();
        else
            doc[index[i]] = nullptr;
    }

    // -----------------------------------------------------------------------
    // Aiguilles (positions + vitesse slider)
    // -----------------------------------------------------------------------
    for (uint8_t i = 0; i < 6; i++)
    {
        Aig *a = canton->getAig(i);

        char keyEtat[4];
        char keyD[4];
        char keyV[4];
        char keyS[4];

        snprintf(keyEtat, sizeof(keyEtat), "s%u", i);
        snprintf(keyD,    sizeof(keyD),    "s%u0", i);
        snprintf(keyV,    sizeof(keyV),    "s%u1", i);
        snprintf(keyS,    sizeof(keyS),    "s%u2", i);

        if (!a)
        {
            doc[keyEtat] = "null";
            doc[keyD]    = "";
            doc[keyV]    = "";
            doc[keyS]    = "";
        }
        else
        {
            doc[keyEtat] = "Actif";
            doc[keyD]    = a->posDroit();
            doc[keyV]    = a->posDevie();
            doc[keyS]    = servoCfg[i].speed;
        }
    }

    // -----------------------------------------------------------------------
    // Paramètres système
    // -----------------------------------------------------------------------
    doc["wifi_on"]        = Settings::wifiOn();
    doc["exploration_on"] = Settings::explorationOn();
    doc["maxSpeed"]       = canton->maxSpeed();
    doc["sensMarche"]     = canton->sensMarche();

    // -----------------------------------------------------------------------
    // Signaux (cible horaire / antihoraire)
    // -----------------------------------------------------------------------
    Signal *sigAH = canton->getSignal(0);
    Signal *sigH  = canton->getSignal(1);

    doc["cibleHoraire"] = sigH  ? sigH->type()  : 0;
    doc["cibleAntiHor"] = sigAH ? sigAH->type() : 0;

    // -----------------------------------------------------------------------
    // BOOSTER — mesures + seuils
    // -----------------------------------------------------------------------
    doc["booster_tension"]      = Booster::tension();
    doc["booster_courant"]      = Booster::courant();
    doc["booster_etat"]         = Booster::etat();
    doc["booster_seuil_libre"]  = Booster::seuilLibre();
    doc["booster_seuil_occupe"] = Booster::seuilOccupe();

    // -----------------------------------------------------------------------
    // Envoi JSON
    // -----------------------------------------------------------------------
    String output;
    serializeJson(doc, output);
    _ws->textAll(output);

    CC_LOG_TRACE("[WebHandler][CC] État complet envoyé aux clients WebSocket\n");
}
