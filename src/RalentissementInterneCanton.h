/*
 * RalentissementInterneCanton.h — Discovery 2026
 * ---------------------------------------------------------------------------
 * Module de gestion du ralentissement interne dans un canton.
 *
 * Rôle :
 *   - détecter le début du canton via le ponctuel d’entrée
 *   - calculer la distance parcourue depuis t0
 *   - déclencher le ralentissement à :
 *         longueur_canton_mm - zone_ralentissement_mm
 *   - appliquer une rampe linéaire vers la vitesse finale (aspect + Δ)
 *   - appliquer la vitesse stricte au ponctuel de sortie
 *
 * IMPORTANT :
 *   - aucune logique métier SNCF ici
 *   - ce module applique uniquement un ralentissement interne
 */

#pragma once

#include <Arduino.h>
#include <Protocol.h>

class Canton;
class Loco;

class RalentissementInterneCanton
{
public:
    RalentissementInterneCanton();

    // Fonction principale appelée dans la boucle du CC
    void executer(Canton *canton, Loco *loco);

private:
    // Indique si un ralentissement interne est en cours
    bool actif;

    // Timestamp du début du canton (ponctuel d’entrée)
    uint32_t t0;

    // Vitesse réelle au moment d’entrer dans le canton
    uint16_t vitesseInitiale;
};
