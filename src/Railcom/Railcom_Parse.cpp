/*
 * Railcom_Parse.cpp — Décodage Railcom et stabilisation de l’adresse
 * -------------------------------------------------------------------
 * Rôle de cette tâche :
 *
 *   1) Attendre un marqueur de début ('\0') dans xQueue1.
 *   2) Lire les 2 octets suivant ce marqueur.
 *   3) Décoder ces octets via le codage 4/8 (table decodeArray).
 *   4) Reconstruire l’adresse DCC (dccAddr[0..1] → temp).
 *   5) Utiliser un buffer circulaire pour vérifier la stabilité de temp.
 *   6) Si l’adresse est stable (buffer homogène) → la pousser dans xQueue2.
 *
 * Objectif pédagogique :
 *   → Séparer clairement :
 *       - la lecture brute (UART) dans receiveData()
 *       - le décodage Railcom dans parseData()
 *       - la mise à disposition de l’adresse dans setAddress()
 */

#include "Railcom.h"
#include "debug_sa.h"

extern RingBuf<uint16_t, NB_ADDRESS_TO_COMPARE> buffer;
extern const byte CH1_ADR_LOW;
extern const byte CH1_ADR_HIGH;

void IRAM_ATTR Railcom::parseData(void *p)
{
    bool start(false);
    int16_t temp(0);
    byte inByte(0);
    uint8_t rxArray[8] = {0};
    uint8_t rxArrayCnt(0);
    byte dccAddr[2] = {0, 0};
    Railcom *pThis = static_cast<Railcom*>(p);

    TickType_t xLastWakeTime = xTaskGetTickCount();

    SA_LOG_TRACE("[Railcom] Tâche parseData() démarrée\n");

    /*
     * Table decodeArray (Railcom 4/8)
     * -------------------------------
     * Railcom encode chaque valeur sur :
     *   - 4 bits utiles
     *   - 4 bits de redondance
     *
     * decodeArray[] permet de :
     *   - convertir un octet 4/8 → valeur utile (0..n)
     *   - renvoyer 255 si le code est invalide
     *
     * Cette table est issue de la spécification Railcom.
     * Elle est copiée telle quelle pour garantir un comportement
     * identique à l’implémentation d’origine.
     */
    const byte decodeArray[] = {
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,64,
        255,255,255,255,255,255,255,51,255,255,255,52,255,53,54,255,
        255,255,255,255,255,255,255,58,255,255,255,59,255,60,55,255,
        255,255,255,63,255,61,56,255,255,62,57,255,255,255,255,255,
        255,255,255,255,255,255,255,36,255,255,255,35,255,34,33,255,
        255,255,255,31,255,30,32,255,255,29,28,255,27,255,255,255,
        255,255,255,25,255,24,26,255,255,23,22,255,21,255,255,255,
        255,37,20,255,19,255,255,255,50,255,255,255,255,255,255,255,
        255,255,255,255,255,255,255,255,255,255,255,255,14,255,13,12,
        255,255,255,255,10,255,9,11,255,255,8,7,255,6,255,255,
        255,255,255,255,4,255,3,5,255,255,2,1,255,0,255,255,
        255,255,15,16,255,17,255,255,255,18,255,255,255,255,255,255,
        255,255,255,255,255,255,43,48,255,255,42,47,255,49,255,255,
        255,255,41,46,255,45,255,255,255,44,255,255,255,255,255,255,
        255,255,66,40,255,39,255,255,255,38,255,255,255,255,255,255,
        255,65,255,255,255,255,255,255,255,255,255,255,255,255,255,255
    };

    auto check_4_8_code = [&]() -> bool
    {
        if (decodeArray[inByte] < 255)
        {
            inByte = decodeArray[inByte];
            return true;
        }
        return false;
    };

    for (;;)
    {
        /*
         * 1) Attente du marqueur de début ('\0')
         *    Tant que ce marqueur n’est pas vu, on ignore les octets.
         */
        do
        {
            if (!pThis->xQueue1)
                break;

            xQueueReceive(pThis->xQueue1, &inByte, pdMS_TO_TICKS(portMAX_DELAY));
            if (inByte == '\0')
                start = true;
        } while (!start);

        start = false;

        /*
         * 2) Lecture des 2 octets suivants :
         *    On récupère les deux octets potentiels d’adresse Railcom.
         */
        for (byte i = 0; i < 2; i++)
        {
            if (pThis->xQueue1 &&
                xQueueReceive(pThis->xQueue1, &inByte, pdMS_TO_TICKS(portMAX_DELAY)) == pdPASS)
            {
                // Railcom : plage de validité avant décodage
                if (inByte >= 0x0F && inByte <= 0xF0)
                {
                    if (check_4_8_code())
                    {
                        rxArray[rxArrayCnt] = inByte;
                        rxArrayCnt++;
                    }
                }
            }
        }

        /*
         * 3) Si on a bien deux octets valides → reconstruction de l’adresse DCC
         */
        if (rxArrayCnt == 2)
        {
            /*
             * Railcom canal 1 encode l’adresse sur deux moitiés :
             *
             *   - CH1_ADR_HIGH (bit 3) → indique que rxArray contient la partie haute
             *   - CH1_ADR_LOW  (bit 2) → indique que rxArray contient la partie basse
             *
             * Le même octet rxArray[0] contient :
             *   - les bits utiles (4/8 décodés)
             *   - les flags HIGH/LOW
             *
             * On recompose l’adresse DCC en combinant :
             *   - rxArray[1] (valeur utile)
             *   - rxArray[0] << 6 (décalage Railcom standard)
             */
            if (rxArray[0] & CH1_ADR_HIGH)
                dccAddr[0] = rxArray[1] | (rxArray[0] << 6);

            if (rxArray[0] & CH1_ADR_LOW)
                dccAddr[1] = rxArray[1] | (rxArray[0] << 6);

            /*
             * Reconstruction de l’adresse DCC :
             * ---------------------------------
             * Railcom transmet l’adresse sur deux octets :
             *
             *   - dccAddr[1] contient la partie haute (avec un offset 128)
             *   - dccAddr[0] contient la partie basse
             *
             * Formule :
             *   (dccAddr[1] - 128) << 8   → partie haute
             *   + dccAddr[0]              → partie basse
             *
             * Si la partie haute est invalide (<0), on ne garde que la basse.
             */
            temp = (dccAddr[1] - 128) << 8;
            if (temp < 0)
                temp = dccAddr[0];
            else
                temp += dccAddr[0];

            /*
             * Stabilisation Railcom :
             * -----------------------
             * Railcom peut produire des valeurs parasites (glitches).
             * Pour éviter de valider une adresse instable, on utilise un
             * buffer circulaire contenant les N dernières adresses lues.
             *
             * Règle :
             *   → L’adresse n’est considérée comme valide que si
             *     TOUTES les entrées du buffer sont identiques à temp.
             *
             * Cela filtre :
             *   - les erreurs ponctuelles
             *   - les transitions instables
             *   - les parasites électriques
             */
            bool testOk = true;
            uint16_t j = 0;

            buffer.pop(j);      // On retire la plus ancienne valeur
            buffer.push(temp);  // On ajoute la nouvelle

            // Vérification : toutes les valeurs doivent être identiques
            do
            {
                if (buffer[j] != temp)
                    testOk = false;
                j++;
            } while (testOk && j <= buffer.size());

            if (testOk)
            {
                if (pThis->xQueue2)
                    xQueueSend(pThis->xQueue2, &temp, 0);

                SA_LOG_INFO("[Railcom] Adresse DCC stable détectée : %u\n", temp);
            }
            else
            {
                SA_LOG_TRACE("[Railcom] Adresse instable (filtrée) : %u\n", temp);
            }
        }

        // 5) Réinitialisation des buffers locaux pour la prochaine trame
        rxArrayCnt = 0;
        for (byte i = 0; i < 2; i++)
            rxArray[i] = 0;

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(25));
    }
}
