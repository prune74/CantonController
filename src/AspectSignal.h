#pragma once

#include <Arduino.h>
#include "Canton.h"
#include "DeductionAspect.h"      // enum ExccAspect
#include "CanMsg.h"
#include "SensEnum.h"
#include "Exploration_Protocol.h" // ExccAspect

/*
 * AspectSignal.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Déduction et envoi des aspects SNCF + feux directionnels.
 *
 * Rôle :
 *   - récupérer les aspects aval SP1 / SM1
 *   - vérifier les aiguilles locales
 *   - déduire l’aspect SNCF local (via DeductionAspect)
 *   - calculer les feux directionnels (FeuxDirection)
 *   - envoyer les aspects / feux / occupation voisins (anti‑spam)
 *
 * Format du tableau signalValue :
 *   signalValue[0] = aspect horaire      (ExccAspect)
 *   signalValue[1] = aspect anti‑horaire (ExccAspect)
 *
 * Chaque aspect est codé sur 1 octet (Option A),
 * conformément au protocole Exploration_Protocol.h.
 */

/*
 * mettreAJourAspectSignal()
 * ---------------------------------------------------------------------------
 * Fonction principale de calcul et d’envoi des aspects SNCF.
 *
 * Paramètres :
 *   - canton       : pointeur vers le canton local
 *   - signalValue  : tableau uint8_t[2] contenant les aspects H / AH
 *
 * Cette fonction applique :
 *   - la déduction locale (DeductionAspect)
 *   - la mise à jour des feux directionnels
 *   - l’envoi conditionnel (anti‑spam)
 */
void mettreAJourAspectSignal(Canton *canton, uint8_t *signalValue);
