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

/**
 * @brief Envoie la topologie SP/SM à EXCC (opcode E4).
 *
 * Lit settings.json, extrait les voisins précédents et suivants,
 * construit la trame E4, puis l’envoie telle quelle à EXCC.
 */
void envoyerTopologieDepuisSettings();

/**
 * @brief Déclenche automatiquement l’envoi de la topologie dès qu’elle est prête.
 *
 * À appeler régulièrement. Une fois envoyée (E4 + E5), la fonction se désactive.
 */
void envoyerTopologieSiPret();

/* =============================================================================
 *  SECTION 2 — SIGNAUX (E5 / E6 / E7)
 * =============================================================================
 */

/**
 * @brief Envoie la configuration des signaux (opcode E5).
 */
void envoyerConfigurationSignauxDepuisSettings();

/**
 * @brief Envoie l’aspect SNCF horaire (opcode E6).
 */
void envoyerAspectSignalHoraire(uint8_t aspect);

/**
 * @brief Envoie l’aspect SNCF anti‑horaire (opcode E7).
 */
void envoyerAspectSignalAntiHoraire(uint8_t aspect);

/* =============================================================================
 *  SECTION 3 — FEUX DIRECTIONNELS (E8 / E9)
 * =============================================================================
 */

/**
 * @brief Envoie les feux directionnels calculés (opcode E8 + E9).
 */
void envoyerFeuxDepuisEtatCourant();

/**
 * @brief Envoie le feu directionnel horaire (opcode E8).
 */
void envoyerFeuDirectionHoraire(uint8_t code);

/**
 * @brief Envoie le feu directionnel anti‑horaire (opcode E9).
 */
void envoyerFeuDirectionAntiHoraire(uint8_t code);

/* =============================================================================
 *  SECTION 4 — AIGUILLES (F0)
 * =============================================================================
 */

/**
 * @brief Envoie l’état logique des aiguilles (opcode F0).
 */
void envoyerAiguillesDepuisEtatCourant();

/* =============================================================================
 *  SECTION 5 — SERVOS (F0 / F1 / F2)
 * =============================================================================
 */

/**
 * @brief Envoie une commande de mouvement réel du servo (opcode F0).
 */
void envoyerServoMove(uint8_t exsaAdresse, uint8_t servoIndex);

/**
 * @brief Envoie la configuration des servos (opcode F1).
 */
void envoyerConfigurationServosDepuisSettings();

/**
 * @brief Envoie une configuration servo individuelle (opcode F1).
 */
void envoyerServoConfig(uint8_t exsaAdresse,
                        uint8_t servoIndex,
                        uint16_t posDroit,
                        uint16_t posDevie,
                        uint16_t speed);

/**
 * @brief Envoie une commande de test servo (opcode F2).
 */
void envoyerServoTest(uint8_t exsaAdresse, uint8_t servoIndex);

/* =============================================================================
 *  SECTION 6 — OCCUPATION (EA)
 * =============================================================================
 */

/**
 * @brief Envoie l’occupation SP1 / SM1 (opcode EA).
 */
void envoyerOccupationDepuisEtatCourant();

/**
 * @brief Envoie directement la valeur d’occupation (2 bits).
 */
void envoyerOccupationVoisins(uint8_t valeur);

/* =============================================================================
 *  SECTION 7 — ASPECTS SNCF (E6 / E7)
 * =============================================================================
 */

/**
 * @brief Envoie les aspects SNCF calculés (opcode E6 + E7).
 */
void envoyerAspectsDepuisEtatCourant();
