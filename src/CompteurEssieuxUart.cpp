#include "CompteurEssieuxUart.h"
#include "SA_EXSA_Protocol.h"
#include "Settings.h"
#include "SatEXSA_Link.h"
#include "Node.h"
#include "debug_sa.h"

/*
 * ============================================================
 *  Variables internes
 * ============================================================
 */

static int s_compteurH = 0;
static int s_compteurAH = 0;

void CompteurEssieuxUart::begin() {
    // Rien à faire
}

void CompteurEssieuxUart::reset() {
    s_compteurH  = 0;
    s_compteurAH = 0;
}

int CompteurEssieuxUart::compteurH()      { return s_compteurH; }
int CompteurEssieuxUart::compteurAH()     { return s_compteurAH; }
int CompteurEssieuxUart::compteurGlobal() { return s_compteurH + s_compteurAH; }

void CompteurEssieuxUart::traiterDelta(int delta, bool coteHoraire)
{
    if (coteHoraire) {
        s_compteurH += delta;
        if (s_compteurH < 0) s_compteurH = 0;
    } else {
        s_compteurAH += delta;
        if (s_compteurAH < 0) s_compteurAH = 0;
    }

    SA_LOG_TRACE("[Essieux] H=%d AH=%d Global=%d\n",
                 s_compteurH, s_compteurAH, s_compteurH + s_compteurAH);
}

void CompteurEssieuxUart::loop()
{
    HardwareSerial& uart = Settings::uart();
    static uint8_t step = 0;
    static uint8_t buffer[3];

    while (uart.available()) {

        uint8_t byte = uart.read();

        switch (step) {

            case 0:
                if (byte == PROTO_SYNC_BYTE) {
                    buffer[0] = byte;
                    step = 1;
                }
                break;

            case 1:
                buffer[1] = byte;
                step = 2;
                break;

            case 2:
                buffer[2] = byte;
                step = 0;

                if (buffer[1] == PROTO_05_DELTA_AXE) {

                    int delta = 0;

                    if (buffer[2] == PROTO_DELTA_PLUS_UN)
                        delta = +1;
                    else if (buffer[2] == PROTO_DELTA_MOINS_UN)
                        delta = -1;

                    bool coteHoraire = SatEXSA_Link::isHoraireOnline();

                    traiterDelta(delta, coteHoraire);
                }

                break;
        }
    }
}
