#pragma once
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "esp_vfs_fat.h"
#include "server_data.h"

namespace PrintServer
{
    #define SD_WRITE_MAX_CHAR_SIZE 8192
    #define SD_MOUNT_POINT "/sdcard"
    
    class SD_Device
    {
    public:
        // the sd_card will be mounted on creation
        SD_Device();
        ~SD_Device()
        {
            esp_vfs_fat_sdcard_unmount(SD_MOUNT_POINT, card);
            ESP_LOGI(DEBUG_NAME, "Card unmounted");

            spi_bus_free((spi_host_device_t)(host.slot));
        }

        float get_capacity()
        {
            return (double)card->csd.capacity * card->csd.sector_size / (1024.0 * 1024.0 * 1024.0);
        }

        int get_freq_mhz()
        {
            return host.max_freq_khz / 1000;
        }

        void unmount()
        {
            esp_vfs_fat_sdcard_unmount(SD_MOUNT_POINT, card);
            ESP_LOGI(DEBUG_NAME, "Card unmounted");

            spi_bus_free((spi_host_device_t)(host.slot));
        } 
    private:
        sdmmc_card_t* card;
        sdmmc_host_t host;
    };
}