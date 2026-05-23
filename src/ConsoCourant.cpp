/* 
 * ============================================================
 *  ConsoCourant.cpp — SA 2026
 * ============================================================
 */

#include "ConsoCourant.h"
#include "SA_EXSA_Protocol.h"
#include "CompteurEssieuxUart.h"
#include "Settings.h"
#include "Node.h"
#include "debug_sa.h"

/* ============================================================
   Constructeur / Destructeur
   ============================================================ */

ConsoCourant::ConsoCourant() {}
ConsoCourant::~ConsoCourant() {}

/* ============================================================
   setup()
   ============================================================ */
void ConsoCourant::setup(Node* node) 
{
    m_node = node;
}

/* ============================================================
   updateEtat(occupePhysique)
   ============================================================ */
void ConsoCourant::updateEtat(bool occupePhysique)
{
    if (!m_node) {
        SA_LOG_ERROR("[ConsoCourant] Erreur : m_node nul\n");
        return;
    }

    int compteur = CompteurEssieuxUart::compteurGlobal();
    bool occupeLogique = occupePhysique || (compteur > 0);

    m_node->busy(occupeLogique);

    SA_LOG_INFO(
        "[ConsoCourant] Canton = %s (phys=%d, essieux=%d)\n",
        occupeLogique ? "OCCUPE" : "LIBRE",
        occupePhysique ? 1 : 0,
        compteur
    );
}

/* ============================================================
   startReceptionUART()
   ============================================================ */
void ConsoCourant::startReceptionUART()
{
    xTaskCreatePinnedToCore(
        ConsoCourant::tacheReceptionUART,
        "UART_RX",
        2048,
        this,
        1,
        nullptr,
        1
    );
}

/* ============================================================
   tacheReceptionUART()
   ============================================================ */
void ConsoCourant::tacheReceptionUART(void* pvParameters)
{
    ConsoCourant* self = static_cast<ConsoCourant*>(pvParameters);
    static uint8_t step = 0;
    static uint8_t buffer[3];

    for (;;)
    {
        if (Settings::uart().available())
        {
            uint8_t byte = Settings::uart().read();

            switch (step)
            {
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

                    if (buffer[1] == PROTO_04_OCCUPATION)
                    {
                        switch (buffer[2])
                        {
                            case PROTO_OCC_ACTIVE:
                                self->updateEtat(true);
                                SA_LOG_INFO("[ConsoCourant] Trame → OCCUPE (phys)\n");
                                break;

                            case PROTO_OCC_LIBRE:
                                self->updateEtat(false);
                                SA_LOG_INFO("[ConsoCourant] Trame → LIBRE (phys)\n");
                                break;

                            default:
                                SA_LOG_WARN("[ConsoCourant] Trame inconnue : %02X\n", buffer[2]);
                                break;
                        }
                    }
                    else {
                        SA_LOG_TRACE("[ConsoCourant] Type de trame ignoré : %02X\n", buffer[1]);
                    }

                    break;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
