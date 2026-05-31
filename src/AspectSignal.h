#pragma once
#include <Arduino.h>
#include "Node.h"
#include "DeductionAspect.h" // version enum ExsaAspect
#include "CanMsg.h"
#include "SensEnum.h"
#include "Discovery_Protocol.h" // pour ExsaAspect

/*
 * signalValue doit être un tableau de uint8_t :
 *   signalValue[0] = aspect horaire  (ExsaAspect)
 *   signalValue[1] = aspect anti-horaire (ExsaAspect)
 *
 * Chaque aspect est codé sur 1 octet (Option A),
 * conformément au protocole Discovery_Protocol.h.
 */
void mettreAJourAspectSignal(Node *node, uint8_t *signalValue);
