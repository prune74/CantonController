#pragma once

#include <Arduino.h>
#include "Canton.h"
#include "DeductionAspect.h" // enum ExccAspect
#include "CC_CAN.h"
#include "SensEnum.h"
#include "Exploration_Protocol.h" // ExccAspect

/*
 * AspectSignal.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Déduction et envoi des aspects SNCF + feux directionnels.
 *
 * Pipeline 2026 :
 *   1) récupérer les aspects aval SP1 / SM1
 *   2) vérifier les aiguilles locales
 *   3) déduire l’aspect SNCF local (BAL)
 *   4) appliquer le mode MANOEUVRE (voie de service)
 *   5) déduire automatiquement le type de mât + MAJ objets Signal
 *   6) calculer les feux directionnels (FeuxDirection)
 *   7) envoyer les aspects / feux / occupation voisins (anti‑spam)
 *
 * Format du tableau signalValue :
 *   signalValue[0] = aspect horaire      (ExccAspect)
 *   signalValue[1] = aspect anti‑horaire (ExccAspect)
 *
 * Chaque aspect est codé sur 1 octet (Option A),
 * conformément au protocole Exploration_Protocol.h.
 *
 * NOTE 2026 :
 *   - Le mode MANOEUVRE ne modifie pas la logique BAL interne.
 *   - Il est appliqué uniquement sur les aspects envoyés à l’EXCC.
 *   - EXCC n’a pas à connaître le mode manœuvre : il affiche seulement
 *     les couleurs correspondant aux aspects reçus.
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
 *   - le mode MANOEUVRE (voie de service)
 *   - la mise à jour du type de mât (Signal::type)
 *   - la mise à jour des feux directionnels
 *   - l’envoi conditionnel (anti‑spam)
 */
void mettreAJourAspectSignal(Canton *canton, uint8_t *signalValue);
