#include "Canton.h"
#include "Exploration_Protocol.h"
#include "debug_cc.h"

/*
 * ============================================================================
 *  updateCompteurDepuisTrame()
 * ---------------------------------------------------------------------------
 *  Met à jour le compteur d’essieux à partir d’une trame EXCC.
 *
 *  Rôle :
 *    - détecter l’opcode PROTO_05_COMPTEUR_ESSIEUX
 *    - mettre à jour le compteur interne du canton
 *
 *  IMPORTANT :
 *    - aucune logique métier
 *    - aucune interprétation RailCom
 *    - aucune supervision ici
 *
 *  La supervision complète est effectuée dans SupervisionEssieux.cpp.
 * ============================================================================
 */
void Canton::updateCompteurDepuisTrame(uint8_t code, uint8_t valeur)
{
    if (code == PROTO_05_COMPTEUR_ESSIEUX)
    {
        setCompteurEssieux(valeur);

        CC_LOG_TRACE("[Canton %u][Compteur][CC] Essieux = %u\n",
                     m_id, valeur);
    }
}
