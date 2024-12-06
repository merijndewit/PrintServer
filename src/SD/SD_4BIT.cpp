//#include "Config.h"
#ifdef SD_INTERFACE_4BIT

#include "SD_Device.h"
#include "esp_log.h"

namespace PrintServer
{
    #define SDMMC_SLOT_CONFIG() {\
        .clk = SD_CLOCK_PIN, \
        .cmd = SD_CMD_PIN, \
        .d0 = SD_D0, \
        .d1 = SD_D1, \
        .d2 = SD_D2, \
        .d3 = SD_D3, \
        .d4 = GPIO_NUM_16, \
        .d5 = GPIO_NUM_17, \
        .d6 = GPIO_NUM_5, \
        .d7 = GPIO_NUM_18, \
        .cd = SDMMC_SLOT_NO_CD, \
        .wp = SDMMC_SLOT_NO_WP, \
        .width   = 4, \
        .flags = 0, \
    }

    SD_Device::SD_Device()
    {
        ESP_LOGI("4-BIT SD","Starting 4Bit bus sd card device");
        esp_vfs_fat_sdmmc_mount_config_t mount_config = 
        {
            .format_if_mount_failed = true,
            .max_files = 5,
            .allocation_unit_size = 16 * 1024,
        };

        ESP_LOGI(DEBUG_NAME, "Initializing SD card");

        ESP_LOGI(DEBUG_NAME, "Using SPI peripheral");

        host = SDMMC_HOST_DEFAULT();
        host.max_freq_khz = SDMMC_FREQ_52M;
        host.flags = SDMMC_HOST_FLAG_4BIT;
        sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG();
        slot_config.width = 4;
        slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

        esp_err_t ret = esp_vfs_fat_sdmmc_mount(SD_MOUNT_POINT, &host, &slot_config, &mount_config, &card);
        if (ret != ESP_OK) 
        {
            server_data_struct.sd_card_detected = 0;
            if (ret == ESP_FAIL) 
            {
                ESP_LOGE(DEBUG_NAME, "Failed to mount filesystem. "
                        "If you want the card to be formatted, set the CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
            } 
            else 
            {
                ESP_LOGE(DEBUG_NAME, "Failed to initialize the card (%s). "
                        "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
            }
            return;
        }
        ESP_LOGI(DEBUG_NAME, "Filesystem mounted");

        sdmmc_card_print_info(stdout, card);
    }

}

#endif // SD_INTERFACE_4BIT