#pragma once

#include <Arduino.h>
#include <map>
#include <string>
#include <Adafruit_MCP23X17.h>

#include "SensEnum.h"
#include "Canton/CantonPeriph.h"
#include "Aig.h"
#include "Signal.h"
#include "Sensor.h"
#include "Loco.h"

/*
 * Canton.h — Architecture Canton 2026
 * ---------------------------------------------------------------------------
 * Représente l’état complet d’un canton ferroviaire :
 *
 *   - Topologie locale : SP1 / SM1 / SP2 / SM2
 *   - Aiguilles logiques (Aig) associées
 *   - Signaux H / AH (objets Signal)
 *   - Capteurs ponctuels (2 par canton)
 *   - Occupation physique + compteur d’essieux
 *   - Feux directionnels (Exploration 2026)
 *   - STOP global
 *   - Mode MANOEUVRE (voie de service)
 *
 * Le canton est l’unité centrale de la logique ferroviaire :
 *   → Le CC calcule les aspects BAL
 *   → AspectSignal applique les règles locales (dont MANOEUVRE)
 *   → EXCC affiche uniquement les couleurs
 *
 * IMPORTANT 2026 :
 *   - Toute la logique métier dépend uniquement de la topologie.
 *   - Le mode MANOEUVRE est un état interne du canton.
 *   - Le mode MANOEUVRE n’est PAS un aspect BAL.
 *   - Le mode MANOEUVRE influence uniquement la logique locale
 *     (ex : BLANC, VIOLET, accès restreint, etc.)
 *
 * LED MANOEUVRE (via Exploration) :
 *   - reflète l’état du mode manœuvre uniquement en exploration
 *   - toujours éteinte en exploitation
 */

// ---------------------------------------------------------------------------
// Paramètres directionnels (feux directionnels + code-barres)
// ---------------------------------------------------------------------------
struct DirectionConfig
{
    bool active = false;
    std::string codeBarre = "";
    std::map<uint16_t, uint8_t> voieDuVoisin;
};

struct DirectionSettings
{
    DirectionConfig H;
    DirectionConfig AH;
};

// ---------------------------------------------------------------------------
// Classe Canton
// ---------------------------------------------------------------------------
class Canton
{
    friend class Exploration;
    friend class CanMsg;

public:
    Canton();
    ~Canton();

    static Canton *s_instance;

    // -----------------------------------------------------------------------
    // MCP23017 — Entrées/sorties locales
    //   (boutons, LED Exploration, LED Manoeuvre)
    // -----------------------------------------------------------------------
    Adafruit_MCP23X17 mcp;
    void initMCP();

    // -----------------------------------------------------------------------
    // Identité du canton
    // -----------------------------------------------------------------------
    void ID(uint16_t id);
    uint16_t ID();

    // -----------------------------------------------------------------------
    // Occupation / Réservation
    // -----------------------------------------------------------------------
    void busy(bool v);              // Occupation physique
    bool busy();
    void reserved(uint16_t addr);   // Réservation RailCom
    uint16_t reserved();
    bool estOccupe();               // Occupé ou réservé

    // -----------------------------------------------------------------------
    // Topologie SP1 / SM1 / SP2 / SM2
    // -----------------------------------------------------------------------
    void SP1_idx(uint8_t idx);
    uint8_t SP1_idx();
    void SM1_idx(uint8_t idx);
    uint8_t SM1_idx();

    void SP2_acces(bool v);
    bool SP2_acces();
    void SP2_busy(bool v);
    bool SP2_busy();

    void SM2_acces(bool v);
    bool SM2_acces();
    void SM2_busy(bool v);
    bool SM2_busy();

    // -----------------------------------------------------------------------
    // Voisins directs
    // -----------------------------------------------------------------------
    CantonPeriph *voisinSP1();
    CantonPeriph *voisinSM1();
    CantonPeriph *voisinSP2();
    CantonPeriph *voisinSM2();

    bool SP1_estAccessible();
    bool SM1_estAccessible();
    bool SP2_estAccessible();
    bool SM2_estAccessible();

    // -----------------------------------------------------------------------
    // Aiguilles logiques
    // -----------------------------------------------------------------------
    void aigRun(uint8_t idx);
    uint8_t getAiguillePosition(uint8_t idx) const;

    // -----------------------------------------------------------------------
    // Signaux (H / AH)
    // -----------------------------------------------------------------------
    uint8_t transitionH();                     // Aspect BAL côté H
    uint8_t transitionAH();                    // Aspect BAL côté AH
    uint8_t transitionAspect(SensDeMarche sens);

    // -----------------------------------------------------------------------
    // Capteurs virtuels EXCC
    // -----------------------------------------------------------------------
    bool readCapteurAH();
    bool readCapteurH();
    bool capteurActif(SensDeMarche sens);
    void overrideCapteur(SensDeMarche sens, bool etat);
    void resetOverrideCapteurs();

    // -----------------------------------------------------------------------
    // Logique ferroviaire interne
    // -----------------------------------------------------------------------
    bool estAccesAutorise(SensDeMarche sens);
    CantonPeriph *prochainVoisin(SensDeMarche sens);
    bool peutEntrerDansVoisin(SensDeMarche sens);
    bool estSortiePossible(SensDeMarche sens);

    // -----------------------------------------------------------------------
    // Déduction du type de mât SNCF (3/4/7/9 feux)
    // -----------------------------------------------------------------------
    uint8_t deduireTypeSignal(SensDeMarche sens) const;

    // Helpers topologiques
    bool estImpasse() const;
    bool estZoneAiguilles() const;
    bool prochainCantonEstDangereux(SensDeMarche sens) const;
    bool aBifurcation(SensDeMarche sens) const;
    bool cantonPrecedentEstEnRalentissement(SensDeMarche sens) const;

    // Besoins d’aspects
    bool besoinRappel(SensDeMarche sens) const;
    bool besoinRalentissement(SensDeMarche sens) const;
    bool besoinCarre(SensDeMarche sens) const;

    // -----------------------------------------------------------------------
    // Vitesse / Sens
    // -----------------------------------------------------------------------
    void maxSpeed(uint8_t v);
    uint8_t maxSpeed();
    void sensMarche(SensDeMarche v);
    SensDeMarche sensMarche();

    // -----------------------------------------------------------------------
    // Feux directionnels (Exploration 2026)
    // -----------------------------------------------------------------------
    void setFeuDirection(SensDeMarche sens, uint8_t valeur);
    uint8_t getFeuDirection(SensDeMarche sens) const;
    void updateFeuDirection(SensDeMarche sens);

    DirectionConfig &directionH() { return direction.H; }
    DirectionConfig &directionAH() { return direction.AH; }

    // -----------------------------------------------------------------------
    // Mode MANOEUVRE (voie de service)
    //   - état interne du canton
    //   - utilisé par AspectSignal
    //   - LED MANOEUVRE gérée par Exploration (exploration uniquement)
    // -----------------------------------------------------------------------
    void setModeManoeuvre(bool v) { m_modeManoeuvre = v; }
    bool modeManoeuvre() const { return m_modeManoeuvre; }

    // -----------------------------------------------------------------------
    // Debug
    // -----------------------------------------------------------------------
    void debugTopologieEtAiguilles();

    // -----------------------------------------------------------------------
    // Initialisation avancée
    // -----------------------------------------------------------------------
    void validateTopology();
    void detectInitialDirection();
    void logInitialState();

    // -----------------------------------------------------------------------
    // Accès contrôlé aux tableaux internes
    // -----------------------------------------------------------------------
    CantonPeriph *getCantonP(uint8_t idx) const
    {
        return (idx < 8) ? cantonP[idx] : nullptr;
    }

    Aig *getAig(uint8_t idx) const
    {
        return (idx < 6) ? aig[idx] : nullptr;
    }

    Signal *getSignal(uint8_t idx) const
    {
        return (idx < 2) ? signal[idx] : nullptr;
    }

    void setSignal(uint8_t idx, Signal *s)
    {
        if (idx < 2)
            signal[idx] = s;
    }

    void setCantonP(uint8_t idx, CantonPeriph *np)
    {
        if (idx < 8)
            cantonP[idx] = np;
    }

    void setAig(uint8_t idx, Aig *a)
    {
        if (idx < 6)
            aig[idx] = a;
    }

    Sensor *getSensor(uint8_t idx)
    {
        return (idx < 2) ? &sensor[idx] : nullptr;
    }

    Sensor *getSensorArray() { return sensor; }

    Loco *getLoco() { return loco; }

    // -----------------------------------------------------------------------
    // Compteur d’essieux
    // -----------------------------------------------------------------------
    void setCompteurEssieux(int v) { m_compteurEssieux = v; }
    int compteurEssieux() const { return m_compteurEssieux; }
    void resetCompteurEssieux() { m_compteurEssieux = 0; }
    void updateCompteurDepuisTrame(uint8_t code, uint8_t valeur);

    // -----------------------------------------------------------------------
    // STOP global Exploration 2026
    // -----------------------------------------------------------------------
    void setStopActive(bool v);
    bool isStopActive() const { return m_stopActive; }

private:
    uint16_t m_id;

    bool m_busy;
    uint16_t m_reserved;

    uint8_t m_SP1_idx;
    uint8_t m_SM1_idx;

    bool m_SP2_acces;
    bool m_SP2_busy;

    bool m_SM2_acces;
    bool m_SM2_busy;

    uint8_t m_maxSpeed;
    SensDeMarche m_sensMarche;

    uint8_t m_feuDirection[2];

    DirectionSettings direction;

    CantonPeriph *cantonP[8];
    Aig *aig[6];
    Signal *signal[2];
    Sensor sensor[2];
    Loco *loco;

    class ConsoCourant *occupation;

    bool m_stopActive = false;

    // -----------------------------------------------------------------------
    // Mode MANOEUVRE (voie de service)
    //   - état interne du canton
    //   - utilisé par AspectSignal
    //   - LED MANOEUVRE gérée par Exploration
    // -----------------------------------------------------------------------
    bool m_modeManoeuvre = false;

    int m_compteurEssieux = 0;
};
