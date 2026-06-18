#pragma once
#include <stdint.h>

/*
 * SatTopologieUART.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Couche de transport UART entre le Canton Controller (CC) et l’Extension
 * Canton Controller (EXCC). Ce module encapsule l’intégralité des trames
 * UART du protocole EXCC :
 *
 *   - E4 : Topologie SP/SM
 *   - E5 : Configuration des signaux
 *   - E6/E7 : Aspects SNCF (H / AH)
 *   - E8/E9 : Feux directionnels (H / AH)
 *   - EA : Occupation SP1 / SM1
 *   - F0/F1/F2 : Commandes servos (mouvement / configuration / test)
 *
 * Rôle :
 *   - fournir une API claire, stable et documentée
 *   - séparer strictement logique métier (Canton, SupervisionCanton, Aig…)
 *     et logique protocolaire (transport UART → EXCC)
 *   - ne contenir AUCUN calcul ferroviaire
 *
 * Les .cpp associés sont dans `SatTopologieUART/` et ne doivent contenir
 * que la traduction CC → EXCC.
 */

/* =============================================================================
 *  SECTION 1 — TOPOLOGIE (E4)
 * =============================================================================
 */

void envoyerTopologieDepuisSettings();
void envoyerTopologieSiPret();

/* =============================================================================
 *  SECTION 2 — SIGNAUX (E5 / E6 / E7)
 * =============================================================================
 */

void envoyerConfigurationSignauxDepuisSettings();
void envoyerAspectSignalHoraire(uint8_t aspect);
void envoyerAspectSignalAntiHoraire(uint8_t aspect);

/* =============================================================================
 *  SECTION 3 — FEUX DIRECTIONNELS (E8 / E9)
 * =============================================================================
 */

void envoyerFeuxDepuisEtatCourant();
void envoyerFeuDirectionHoraire(uint8_t code);
void envoyerFeuDirectionAntiHoraire(uint8_t code);

/* =============================================================================
 *  SECTION 4 — AIGUILLES (F0)
 * =============================================================================
 */

void envoyerAiguillesDepuisEtatCourant();

/* =============================================================================
 *  SECTION 5 — SERVOS (F0 / F1 / F2)
 * =============================================================================
 */

/**
 * @brief Envoie une commande de mouvement réel du servo (opcode F0).
 * @param servoIndex Index du servo (0..5)
 * @param direction  0 = droit, 1 = dévié
 */
void envoyerServoMove(uint8_t servoIndex, uint8_t direction);

/**
 * @brief Envoie la configuration des servos (opcode F1) depuis settings.json.
 */
void envoyerConfigurationServosDepuisSettings();

/**
 * @brief Envoie une configuration servo individuelle (opcode F1).
 */
void envoyerServoConfig(uint8_t servoIndex,
                        uint16_t posDroit,
                        uint16_t posDevie,
                        uint16_t speed);

/**
 * @brief Envoie une commande de test servo (opcode F2).
 */
void envoyerServoTest(uint8_t servoIndex);

/* =============================================================================
 *  SECTION 6 — OCCUPATION (EA)
 * =============================================================================
 */

void envoyerOccupationDepuisEtatCourant();
void envoyerOccupationVoisins(uint8_t valeur);

/* =============================================================================
 *  SECTION 7 — ASPECTS SNCF (E6 / E7)
 * =============================================================================
 */

void envoyerAspectsDepuisEtatCourant();
