#pragma once

/*
 * Node.h — Interface publique du canton (Node)
 * ------------------------------------------------------------
 * Représente un CANTON du réseau.
 *
 * Notes 2026 :
 *  - Le SA ne pilote plus aucun servo.
 *  - Les aiguilles sont 100 % logiques (Aig).
 *  - EXSA pilote physiquement les servos via PCA9685.
 *  - Node stocke uniquement la logique ferroviaire.
 */

#include <Arduino.h>
#include <map>
#include <string>

#include "SensEnum.h"
#include "Node/NodePeriph.h"
#include "Aig.h"
#include "Signal.h"
#include "Sensor.h"
#include "Loco.h"

/* ---------------------------------------------------------------------------
 * Rôle ferroviaire du canton
 * ------------------------------------------------------------------------- */
enum CantonRole
{
    ROLE_PLEINE_VOIE = 0,
    ROLE_BAL,
    ROLE_GARE,
    ROLE_ENTREE_GARE,
    ROLE_SORTIE_GARE,
    ROLE_MANOEUVRE,
    ROLE_SERVICE
};

/* ---------------------------------------------------------------------------
 * Structures FeuxDirection (H / AH)
 * ------------------------------------------------------------------------- */
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

/* ---------------------------------------------------------------------------
 * Classe Node
 * ------------------------------------------------------------------------- */
class Node
{
    friend class Discovery;
    friend class CanMsg;

public:
    Node();
    ~Node();

    // Singleton SA = un seul Node
    static Node* s_instance;

    /* Identité */
    void ID(uint16_t id);
    uint16_t ID();

    /* Occupation / Réservation */
    void busy(bool v);
    bool busy();
    void reserved(uint16_t addr);
    uint16_t reserved();
    bool estOccupe();

    /* Rôle ferroviaire */
    void setRole(CantonRole role);
    CantonRole getRole();
    bool roleAutoriseAcces(SensDeMarche sens);
    bool roleImposeAvertissement();
    bool roleImposeManoeuvre();

    /* Topologie SP1 / SM1 / SP2 / SM2 */
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

    /* Masques d’aiguilles */
    void masqueAig(uint8_t v);
    uint8_t masqueAig();
    void masqueAigSP2(uint8_t v);
    uint8_t masqueAigSP2();
    void masqueAigSM2(uint8_t v);
    uint8_t masqueAigSM2();

    /* Voisins directs */
    NodePeriph* voisinSP1();
    NodePeriph* voisinSM1();
    NodePeriph* voisinSP2();
    NodePeriph* voisinSM2();

    bool SP1_estAccessible();
    bool SM1_estAccessible();
    bool SP2_estAccessible();
    bool SM2_estAccessible();

    /* Aiguilles (LOGIQUES) */
    void aigRun(uint8_t idx);
    uint8_t getAiguillePosition(uint8_t idx) const;

    /* Signaux (H / AH) */
    void applyRoleDefaults();
    uint8_t transitionH();
    uint8_t transitionAH();
    uint8_t transitionAspect(SensDeMarche sens);

    /* Capteurs virtuels EXSA */
    bool readCapteurAH();
    bool readCapteurH();
    bool capteurActif(SensDeMarche sens);
    void overrideCapteur(SensDeMarche sens, bool etat);
    void resetOverrideCapteurs();

    /* Logique métier ferroviaire */
    bool estAccesAutorise(SensDeMarche sens);
    bool aiguillesConformes(uint8_t masque);
    NodePeriph* prochainVoisin(SensDeMarche sens);
    bool peutEntrerDansVoisin(SensDeMarche sens);
    bool estSortiePossible(SensDeMarche sens);

    /* Vitesse / Sens */
    void maxSpeed(uint8_t v);
    uint8_t maxSpeed();
    void sensMarche(SensDeMarche v);
    SensDeMarche sensMarche();

    /* Feux directionnels */
    void setFeuDirection(SensDeMarche sens, uint8_t valeur);
    uint8_t getFeuDirection(SensDeMarche sens) const;
    void updateFeuDirection(SensDeMarche sens);

    /* API FeuxDirection pour Settings_JSON */
    DirectionConfig& directionH() { return direction.H; }
    DirectionConfig& directionAH() { return direction.AH; }

    /* Debug */
    void debugTopologieEtAiguilles();

    /* Initialisation avancée */
    void validateTopology();
    void detectInitialDirection();
    void logInitialState();

    /* Accès contrôlé */
    NodePeriph* getNodeP(uint8_t idx) {
        return (idx < 8) ? nodeP[idx] : nullptr;
    }

    Aig* getAig(uint8_t idx) {
        return (idx < 6) ? aig[idx] : nullptr;
    }

    Signal* getSignal(uint8_t idx) {
        return (idx < 2) ? signal[idx] : nullptr;
    }

    // 🔥 Ajout indispensable pour main.cpp
    void setSignal(uint8_t idx, Signal* s) {
        if (idx < 2)
            signal[idx] = s;
    }

    Sensor* getSensor(uint8_t idx) {
        return (idx < 2) ? &sensor[idx] : nullptr;
    }

    Sensor* getSensorArray() { return sensor; }

    Loco* getLoco() {
        return loco;
    }

    /* STOP global Discovery 2026 */
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

    CantonRole m_role;

    uint8_t m_feuDirection[2];

    DirectionSettings direction;

    NodePeriph* nodeP[8];
    Aig*        aig[6];
    Signal*     signal[2];
    Sensor      sensor[2];
    Loco*       loco;

    class ConsoCourant* occupation;

    bool m_stopActive = false;
};
