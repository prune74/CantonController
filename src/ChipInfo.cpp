#include "ChipInfo.h"

void ChipInfo::print()
{
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    Serial.printf("\n\n--------------- Infos ESP32 ----------------------\n");
    Serial.printf("[ChipInfo][CC] Arduino Release : %s\n", ARDUINO_ESP32_RELEASE);
    Serial.printf("[ChipInfo][CC] Chip Revision   : %d\n", chip_info.revision);
    Serial.printf("[ChipInfo][CC] SDK             : %s\n", ESP.getSdkVersion());
    Serial.printf("[ChipInfo][CC] CPU freq        : %d MHz\n", ESP.getCpuFreqMHz());
    Serial.printf("[ChipInfo][CC] CPU cores       : %d\n", chip_info.cores);

    uint32_t flashMB = spi_flash_get_chip_size() / (1024 * 1024);
    Serial.printf("[ChipInfo][CC] Flash size      : %d MB %s\n",
                  flashMB,
                  (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "(embedded)" : "(external)");

    Serial.printf("[ChipInfo][CC] APB CLOCK       : %d MHz\n", APB_CLK_FREQ / 1000000);
    Serial.printf("[ChipInfo][CC] Free RAM        : %ld bytes\n", (long)ESP.getFreeHeap());
    Serial.printf("-------------------------------------------------\n\n");
}
