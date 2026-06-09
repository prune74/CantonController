#include "CompteurEssieuxUart.h"
#include "Exploration_Protocol.h"
#include "Settings.h"
#include "SatEXSA_Link.h"
#include "Canton.h"
#include "debug_sa.h"

/*
 * ============================================================
 *  Variables internes
 * ============================================================
 */

static int s_compteurH = 0;
static int s_compteurAH = 0;

/*
 * ============================================================
 *  API publique
 * ============================================================
 */

void CompteurEssieuxUart::reset()
{
    s_compteurH = 0;
    s_compteurAH = 0;
}

int CompteurEssieuxUart::compteurH() { return s_compteurH; }
int CompteurEssieuxUart::compteurAH() { return s_compteurAH; }
int CompteurEssieuxUart::compteurGlobal() { return s_compteurH + s_compteurAH; }

/*
 * ============================================================
 *  traiterDelta() — logique métier
 * ============================================================
 */

void CompteurEssieuxUart::traiterDelta(int delta, bool coteHoraire)
{
    if (coteHoraire)
    {
        s_compteurH += delta;
        if (s_compteurH < 0)
            s_compteurH = 0;
    }
    else
    {
        s_compteurAH += delta;
        if (s_compteurAH < 0)
            s_compteurAH = 0;
    }

    SA_LOG_TRACE("[Essieux] H=%d AH=%d Global=%d\n",
                 s_compteurH, s_compteurAH, s_compteurH + s_compteurAH);
}

/*
 * ============================================================
 *  onDelta() — callback appelé par SA_UartRx
 * ============================================================
 *
 * Trame reçue :
 *   [SYNC][0x05][index_exsa][delta]
 *
 * index_exsa :
 *   0 = EXSA Horaire
 *   1 = EXSA AntiHoraire
 *
 */
void CompteurEssieuxUart::onDelta(uint8_t index_exsa, uint8_t code)
{
    int delta = 0;

    if (code == PROTO_DELTA_PLUS_UN)
        delta = +1;
    else if (code == PROTO_DELTA_MOINS_UN)
        delta = -1;

    bool coteHoraire = (index_exsa == 0);

    traiterDelta(delta, coteHoraire);
}
