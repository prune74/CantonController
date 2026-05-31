#pragma once
#include <stdint.h>

class Node;

class SupervisionAiguilles
{
public:
    static void begin(Node* node);

    static void onPosition(uint8_t index_exsa,
                           uint8_t idAig,
                           uint8_t etat,
                           uint8_t masque);

private:
    static Node* s_node;
};
