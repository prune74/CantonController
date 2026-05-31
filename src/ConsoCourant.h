#pragma once

#include <stdint.h>

class Node;

/*
 * ConsoCourant — SA 2026
 * ----------------------
 * - Représente l’occupation physique d’un canton.
 * - L’info vient d’EXSA via PROTO_04.
 * - Fusionne occupation physique + compteur essieux.
 * - Met à jour node->busy().
 */

class ConsoCourant
{
public:
    ConsoCourant();
    ~ConsoCourant();

    // Singleton : instance unique du SA
    static ConsoCourant* s_instance;
    
    // Associe ce capteur virtuel à un canton
    void setup(Node* node);

    // Appelé par SA_UartRx (PROTO_04)
    static void onOccupation(uint8_t index_exsa, uint8_t code);

    // Mise à jour interne
    void updateEtat(bool occupePhysique);

private:
    Node* m_node = nullptr;
};
