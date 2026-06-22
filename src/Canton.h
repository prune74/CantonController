#pragma once

#include <Arduino.h>
#include <map>
#include <string>
#include <Adafruit_MCP23X17.h>

#include "SensEnum.h"
#include "Canton/CantonPeriph.h"
#include "Aig.h"
#include "Signal.h"
#include "Loco.h"

/*
 * Canton.h — Architecture Canton 2026 (version neuve)
 * ---------------------------------------------------------------------------
 * Représente l’état complet d’un canton ferroviaire.
 *
 * Le CC ne lit plus les capteurs physiques :
 *   → EXCC lit les capteurs
 *   → EXCC envoie les états ponctuels H/AH
 *   → Le CC stocke uniquement l’état logique
 *
 * Le canton reste l’unité centrale de la logique ferroviaire :
 *   - Topologie SP1 / SM1 / SP2 / SM2
 *   - Aiguilles logiques
 *   - Signaux H / AH
 *   - Occupation (courant + essieux)
 *   - Feux directionnels
 *   - Mode MANOEUVRE
 */

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

class Canton
{
    friend class Exploration;
    friend class CanMsg;

public:
    Canton();
    ~Canton();

    static Canton *s_instance;

    // -----------------------------------------------------------------------
    // MCP23017 — E/S locales
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
    uint8_t transitionH();
    uint8_t transitionAH();
    uint8_t transitionAspect(SensDeMarche sens);

    // -----------------------------------------------------------------------
    // Logique ferroviaire interne
    // -----------------------------------------------------------------------
    bool estAccesAutorise(SensDeMarche sens);
    CantonPeriph *prochainVoisin(SensDeMarche sens);
    bool peutEntrerDansVoisin(SensDeMarche sens);
    bool estSortiePossible(SensDeMarche sens);

    uint8_t deduireTypeSignal(SensDeMarche sens) const;

    bool estImpasse() const;
    bool estZoneAiguilles() const;
    bool prochainCantonEstDangereux(SensDeMarche sens) const;
    bool aBifurcation(SensDeMarche sens) const;
    bool cantonPrecedentEstEnRalentissement(SensDeMarche sens) const;

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
    // Feux directionnels
    // -----------------------------------------------------------------------
    void setFeuDirection(SensDeMarche sens, uint8_t valeur);
    uint8_t getFeuDirection(SensDeMarche sens) const;
    void updateFeuDirection(SensDeMarche sens);

    DirectionConfig &directionH() { return direction.H; }
    DirectionConfig &directionAH() { return direction.AH; }

    // -----------------------------------------------------------------------
    // Mode MANOEUVRE
    // -----------------------------------------------------------------------
    void setModeManoeuvre(bool v) { m_modeManoeuvre = v; }
    bool modeManoeuvre() const { return m_modeManoeuvre; }

    // -----------------------------------------------------------------------
    // STOP global Exploration 2026
    // -----------------------------------------------------------------------
    void setStopActive(bool v);
    bool isStopActive() const { return m_stopActive; }

    // -----------------------------------------------------------------------
    // Capteurs ponctuels (H / AH) — état logique envoyé par EXCC
    // -----------------------------------------------------------------------
    void setPonctuelH(bool v) { m_ponctuelH = v; }
    void setPonctuelAH(bool v) { m_ponctuelAH = v; }

    bool ponctuelH() const { return m_ponctuelH; }
    bool ponctuelAH() const { return m_ponctuelAH; }

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

    Loco *getLoco() { return loco; }

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

    // -----------------------------------------------------------------------
    // Initialisation avancée
    // -----------------------------------------------------------------------
    void validateTopology();

    // -----------------------------------------------------------------------
    // Debug
    // -----------------------------------------------------------------------
    void debugTopologieEtAiguilles();

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
    Loco *loco;

    class Occupation *occupation;

    bool m_stopActive = false;
    bool m_modeManoeuvre = false;

    int m_compteurEssieux = 0;

    // -----------------------------------------------------------------------
    // Capteurs ponctuels (H / AH)
    // -----------------------------------------------------------------------
    bool m_ponctuelH = false;
    bool m_ponctuelAH = false;
};
