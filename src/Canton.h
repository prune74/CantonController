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
 * Canton.h — Gestion Canton 2026
 * ---------------------------------------------------------------------------
 * Représente un canton complet :
 *   - topologie SP1 / SM1 / SP2 / SM2
 *   - aiguilles logiques
 *   - signaux H / AH
 *   - capteurs ponctuels
 *   - occupation physique + essieux
 *   - feux directionnels
 *   - STOP global
 *
 * Ce module est le cœur de la logique métier ferroviaire locale.
 *
 * IMPORTANT 2026 :
 *   - Les rôles ferroviaires ont été supprimés.
 *   - Toute la logique métier dépend désormais uniquement de la topologie.
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
    // MCP23017
    // -----------------------------------------------------------------------
    Adafruit_MCP23X17 mcp;
    void initMCP();

    // -----------------------------------------------------------------------
    // Identité
    // -----------------------------------------------------------------------
    void ID(uint16_t id);
    uint16_t ID();

    // -----------------------------------------------------------------------
    // Occupation / Réservation
    // -----------------------------------------------------------------------
    void busy(bool v);
    bool busy();
    void reserved(uint16_t addr);
    uint16_t reserved();
    bool estOccupe();

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
    // Masques d’aiguilles
    // -----------------------------------------------------------------------
    void masqueAig(uint8_t v);
    uint8_t masqueAig();
    void masqueAigSP2(uint8_t v);
    uint8_t masqueAigSP2();
    void masqueAigSM2(uint8_t v);
    uint8_t masqueAigSM2();

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
    void applyRoleDefaults();  // désormais vide
    uint8_t transitionH();
    uint8_t transitionAH();
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
    // Logique métier ferroviaire
    // -----------------------------------------------------------------------
    bool estAccesAutorise(SensDeMarche sens);
    bool aiguillesConformes(uint8_t masque);
    CantonPeriph *prochainVoisin(SensDeMarche sens);
    bool peutEntrerDansVoisin(SensDeMarche sens);
    bool estSortiePossible(SensDeMarche sens);

    // -----------------------------------------------------------------------
    // Vitesse / Sens
    // -----------------------------------------------------------------------
    void maxSpeed(uint8_t v);
    uint8_t maxSpeed();
    void sensMarche(SensDeMarche v);
    SensDeMarche sensMarche();

    // -----------------------------------------------------------------------
    // Feux directionnels
    // -----------------------------------------------------------------------
    void setFeuDirection(SensDeMarche sens, uint8_t valeur);
    uint8_t getFeuDirection(SensDeMarche sens) const;
    void updateFeuDirection(SensDeMarche sens);

    DirectionConfig &directionH() { return direction.H; }
    DirectionConfig &directionAH() { return direction.AH; }

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
    CantonPeriph *getCantonP(uint8_t idx)
    {
        return (idx < 8) ? cantonP[idx] : nullptr;
    }

    Aig *getAig(uint8_t idx)
    {
        return (idx < 6) ? aig[idx] : nullptr;
    }

    Signal *getSignal(uint8_t idx)
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

    uint8_t m_masqueAig;

    uint8_t m_SP1_idx;
    uint8_t m_SM1_idx;

    bool m_SP2_acces;
    bool m_SP2_busy;

    bool m_SM2_acces;
    bool m_SM2_busy;

    uint8_t m_masqueAigSP2;
    uint8_t m_masqueAigSM2;

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

    int m_compteurEssieux = 0;
};
