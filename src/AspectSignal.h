#pragma once
#include <Arduino.h>
#include "Canton.h"
#include "DeductionAspect.h" // version enum ExsaAspect
#include "CanMsg.h"
#include "SensEnum.h"
#include "Exploration_Protocol.h" // pour ExsaAspect

/*
 * signalValue doit être un tableau de uint8_t :
 *   signalValue[0] = aspect horaire  (ExsaAspect)
 *   signalValue[1] = aspect anti-horaire (ExsaAspect)
 *
 * Chaque aspect est codé sur 1 octet (Option A),
 * conformément au protocole Exploration_Protocol.h.
 */
void mettreAJourAspectSignal(Canton *canton, uint8_t *signalValue);
