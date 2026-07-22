/*
 * Canton_RechercheWagon.cpp — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Recherche séquentielle des wagons d’une rame, compatible RailCom.
 *
 * Règles physiques :
 *   - On n’active JAMAIS RailCom d’un wagon dans CC0 (collision avec la loco)
 *   - On cherche uniquement dans SP1, SP2, SM1, SM2
 *   - Un seul wagon à la fois (index)
 *   - Si aucun voisin ne détecte le wagon → il est dans CC0 → wagon validé
 */

#include "Canton.h"
#include "CommandeDCC.h"
#include "SupervisionCAN.h"
#include "CC_CAN_CLF.h"
#include "Railcom.h"
#include "debug_cc.h"

/* ---------------------------------------------------------------------------
 * Wrapper Canton → CommandeDCC
 * ---------------------------------------------------------------------------*/
void Canton::envoyerCommande_DCC_WAGON_RAILCOM(uint16_t wagonAddr, bool enable)
{
    envoyerCommande_DCC_WAGON_RAILCOM(wagonAddr, enable);
}

/* ---------------------------------------------------------------------------
 * Activation RailCom d’un seul wagon (index)
 * ---------------------------------------------------------------------------*/
static void activerRailcomWagon(Canton *canton, uint16_t wagonAddr, bool enable)
{
    CC_LOG_INFO("[RechercheWagon][CC] Wagon %u → RailCom %s\n",
                wagonAddr, enable ? "ON" : "OFF");

    canton->envoyerCommande_DCC_WAGON_RAILCOM(wagonAddr, enable);
}

/* ---------------------------------------------------------------------------
 * Propagation vers SP1/SP2/SM1/SM2
 * ---------------------------------------------------------------------------*/
static void propagerVersVoisins(Canton *canton, uint16_t locoId, uint8_t index)
{
    CantonPeriph *voisins[4] = {
        canton->voisinSP1(),
        canton->voisinSP2(),
        canton->voisinSM1(),
        canton->voisinSM2()
    };

    bool auMoinsUnVoisin = false;

    for (auto *v : voisins)
    {
        if (!v) continue;
        auMoinsUnVoisin = true;

        CC_LOG_INFO("[RechercheWagon][CC] Propagation vers CC %u (index=%u)\n",
                    v->ID(), index);

        SupervisionCAN::envoyerRechercheWagon(canton, v->ID(), locoId, index);
    }

    if (!auMoinsUnVoisin)
    {
        CC_LOG_WARN("[RechercheWagon][CC] Aucun voisin → wagon considéré dans CC0\n");
        canton->validerWagonDansCC0(locoId, index);
    }
}

/* ---------------------------------------------------------------------------
 * Déclenchement local (CC0)
 * ---------------------------------------------------------------------------*/
void Canton::declencherRechercheDirectionnelle()
{
    uint16_t locoId = reserved();
    if (locoId == 0)
    {
        CC_LOG_WARN("[RechercheWagon][CC] Impossible : aucune loco réservée\n");
        return;
    }

    const auto &wagons = CC_CAN_CLF::getComposition(locoId);
    if (wagons.empty())
    {
        CC_LOG_WARN("[RechercheWagon][CC] Composition vide pour rame %u\n", locoId);
        return;
    }

    CC_LOG_INFO("[RechercheWagon][CC] Déclenchement recherche pour rame %u\n", locoId);

    // ⭐ CC0 NE CHERCHE PAS (collision RailCom avec la loco)
    // → CC0 propage directement vers SP1/SP2/SM1/SM2
    uint8_t index = 0;
    propagerVersVoisins(this, locoId, index);
}

/* ---------------------------------------------------------------------------
 * Réception depuis un voisin
 * ---------------------------------------------------------------------------*/
void Canton::traiterRechercheDirectionnelleRecue(uint16_t locoId, uint8_t index)
{
    const auto &wagons = CC_CAN_CLF::getComposition(locoId);
    if (index >= wagons.size())
    {
        CC_LOG_WARN("[RechercheWagon][CC] Index hors limite pour rame %u\n", locoId);
        return;
    }

    uint16_t wagon = wagons[index];

    CC_LOG_INFO("[RechercheWagon][CC] Recherche reçue : rame=%u wagon=%u index=%u\n",
                locoId, wagon, index);

    // ⭐ Ce canton active RailCom (car ici pas de loco → pas de collision)
    activerRailcomWagon(this, wagon, true);

    // Lecture RailCom locale
    bool detecte = this->railcomDetecte(wagon);

    if (detecte)
    {
        CC_LOG_INFO("[RechercheWagon][CC] Wagon %u détecté dans ce canton\n", wagon);

        activerRailcomWagon(this, wagon, false); // OFF

        // Wagon validé → index++
        uint8_t nextIndex = index + 1;

        if (nextIndex >= wagons.size())
        {
            CC_LOG_INFO("[RechercheWagon][CC] Tous les wagons ont été validés\n");
            return;
        }

        // Retour vers CC0 pour lancer la recherche du wagon suivant
        SupervisionCAN::envoyerRechercheWagon(this, Canton::s_instance->ID(), locoId, nextIndex);
        return;
    }

    // ⭐ Non détecté → propagation vers voisins
    activerRailcomWagon(this, wagon, false); // OFF avant propagation
    propagerVersVoisins(this, locoId, index);
}

/*
 * validerWagonDansCC0()
 * ---------------------
 * Appelé quand aucun voisin n’a détecté le wagon.
 * Cela signifie physiquement :
 *   → Le wagon est dans CC0 (canton local)
 *   → On valide ce wagon
 *   → On passe au wagon suivant
 */
void Canton::validerWagonDansCC0(uint16_t locoId, uint8_t index)
{
    const auto &wagons = CC_CAN_CLF::getComposition(locoId);
    if (index >= wagons.size())
        return;

    uint16_t wagon = wagons[index];

    CC_LOG_INFO("[CC0] Wagon %u considéré dans CC0\n", wagon);

    uint8_t nextIndex = index + 1;

    if (nextIndex < wagons.size())
    {
        CC_LOG_INFO("[CC0] Passage au wagon suivant : index=%u\n", nextIndex);
        SupervisionCAN::envoyerRechercheWagon(this, this->ID(), locoId, nextIndex);
    }
    else
    {
        CC_LOG_INFO("[CC0] Tous les wagons validés pour la rame %u\n", locoId);
    }
}

/*
 * railcomDetecte()
 * ----------------
 * Retourne true si l’adresse RailCom reçue par EXCC correspond
 * à l’adresse du wagon recherché.
 */
bool Canton::railcomDetecte(uint16_t wagonAddr)
{
    uint16_t adr = Railcom::address();

    CC_LOG_INFO("[RailCom][CC] Lecture RailCom : reçu=%u, attendu=%u\n",
                adr, wagonAddr);

    return (adr == wagonAddr);
}

