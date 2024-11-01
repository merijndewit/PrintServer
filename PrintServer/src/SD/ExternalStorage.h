#pragma once
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "driver/sdmmc_types.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"

#define USE_SPI_MODE

namespace PrintServer
{
    class ExternalStorage
    {
    public:
        ExternalStorage();
    private:
        sdspi_dev_handle_t sd_handle;
        sdmmc_card_t sd_card_handle;
    };
}