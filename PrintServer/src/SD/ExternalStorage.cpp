#include "ExternalStorage.h"
#include "Config.h"

namespace PrintServer
{
    ExternalStorage::ExternalStorage()
    {
        sdspi_device_config_t sd_device_config 
        {
            .host_id = SPI2_HOST, 
            .gpio_cs = GPIO_NUM_13, 
            .gpio_cd = GPIO_NUM_NC, 
            .gpio_wp = GPIO_NUM_NC, 
            .gpio_int = GPIO_NUM_NC, 
            .gpio_wp_polarity = 0,
        };

        sdspi_host_init_device(&sd_device_config, &sd_handle);
    
        sdmmc_host_t sdmmc_host_config
        {
            .flags = SDMMC_HOST_FLAG_SPI | SDMMC_HOST_FLAG_DEINIT_ARG, 
            .slot = sd_handle, 
            .max_freq_khz = SDMMC_FREQ_DEFAULT, 
            .io_voltage = 3.3f, 
            .init = &sdspi_host_init, 
            .set_bus_width = NULL, 
            .get_bus_width = NULL, 
            .set_bus_ddr_mode = NULL, 
            .set_card_clk = &sdspi_host_set_card_clk, 
            .set_cclk_always_on = NULL, 
            .do_transaction = &sdspi_host_do_transaction, 
            .deinit_p = &sdspi_host_remove_device, 
            .io_int_enable = &sdspi_host_io_int_enable, 
            .io_int_wait = &sdspi_host_io_int_wait, 
            .command_timeout_ms = 0, 
            .get_real_freq = &sdspi_host_get_real_freq, 
            .input_delay_phase = SDMMC_DELAY_PHASE_0, 
            .set_input_delay = NULL, 
            .get_dma_info = &sdspi_host_get_dma_info,
        };

        sdmmc_card_init(&sdmmc_host_config, &sd_card_handle);
    };
}
