#pragma once
#include <Arduino.h>
#include "Discovery_Protocol.h" // pour ExsaAspect

/*************************************************************************************
 * Aspects SNCF utilisés par le SA (version EXSA 2.1 simplifiée)
 * Ces aspects correspondent EXACTEMENT aux aspects que l’EXSA sait afficher.
 *************************************************************************************/

using Aspect = ExsaAspect;

/*************************************************************************************
 * Déduction d’aspect (version Option A)
 *************************************************************************************/
Aspect deduireAspectDepuisAval(ExsaAspect aval, bool voieDevie);
