#pragma once
#include <stdint.h>

class CompteurEssieuxUart
{
public:

    static void reset();

    static int compteurH();
    static int compteurAH();
    static int compteurGlobal();

    static void traiterDelta(int delta, bool coteHoraire);

    /*
     * Nouveau callback appelé par SA_UartRx
     *
     * Trame reçue :
     *   [SYNC][PROTO_05_DELTA_AXE][index_exsa][delta]
     *
     * index_exsa :
     *   0 = EXSA Horaire
     *   1 = EXSA AntiHoraire
     */
    static void onDelta(uint8_t index_exsa, uint8_t code);
};
