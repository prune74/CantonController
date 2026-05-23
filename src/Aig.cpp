/*
  Aig.cpp — Version 2026 (CLEAN)
  ------------------------------------------------------------
  Implémentation d’une AIGUILLE LOGIQUE.

  Le SA ne pilote aucun servo :
  - EXSA (H ou AH) effectue le mouvement réel via PCA9685.
  - Cette classe stocke uniquement l’état LOGIQUE :
        * positions droite / déviée
        * position logique courante
        * index EXSA côté H / AH
  - Le SA transmet ces valeurs à EXSA via RS485 (F0/F1/F2).
  ------------------------------------------------------------
*/

#include "Aig.h"

/*-------------------------------------------------------------
   Constructeur
   Initialise toutes les valeurs LOGIQUES d’une aiguille.
--------------------------------------------------------------*/
Aig::Aig() :
    m_id(0),
    m_posDroit(1500),
    m_posDevie(1500),
    m_minPos(800),
    m_maxPos(2400),
    m_estDroit(true),
    m_curPos(1500),
    m_nodePdroitIdx(0),
    m_nodePdevieIdx(0)
{}

/*-------------------------------------------------------------
   Destructeur
--------------------------------------------------------------*/
Aig::~Aig() {}

/*-------------------------------------------------------------
   Identifiant logique
--------------------------------------------------------------*/
void Aig::ID(uint8_t id) { m_id = id; }
uint8_t Aig::ID() const { return m_id; }

/*-------------------------------------------------------------
   État logique (droite / déviée)
--------------------------------------------------------------*/
void Aig::estDroit(bool pos) { m_estDroit = pos; }
bool Aig::estDroit() const { return m_estDroit; }

/*-------------------------------------------------------------
   Positions logiques droite / déviée
--------------------------------------------------------------*/
void Aig::posDroit(uint16_t pos)
{
  if (pos > m_minPos && pos < m_maxPos)
    m_posDroit = pos;
}

void Aig::posDevie(uint16_t pos)
{
  if (pos > m_minPos && pos < m_maxPos)
    m_posDevie = pos;
}

uint16_t Aig::posDroit() const { return m_posDroit; }
uint16_t Aig::posDevie() const { return m_posDevie; }

/*-------------------------------------------------------------
   Position logique courante
   (EXSA effectuera le mouvement réel)
--------------------------------------------------------------*/
void Aig::curPos(uint16_t pos)
{
  if (pos > m_minPos && pos < m_maxPos)
    m_curPos = pos;
}

uint16_t Aig::curPos() const { return m_curPos; }

/*-------------------------------------------------------------
   Index EXSA côté H (SP1)
--------------------------------------------------------------*/
void Aig::nodePdroitIdx(uint8_t idx) { m_nodePdroitIdx = idx; }
uint8_t Aig::nodePdroitIdx() const { return m_nodePdroitIdx; }

/*-------------------------------------------------------------
   Index EXSA côté AH (SM1)
--------------------------------------------------------------*/
void Aig::nodePdevieIdx(uint8_t idx) { m_nodePdevieIdx = idx; }
uint8_t Aig::nodePdevieIdx() const { return m_nodePdevieIdx; }
/* ------------------------------------------------------------
  Fin de Aig.cpp
  ------------------------------------------------------------
*/