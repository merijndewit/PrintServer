#include "Core.h"
#include "spi_flash_mmap.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

extern "C" void app_main() 
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    PrintServer::Core core;
    
    while (core.is_running())
    {
       core.update();
    }
}