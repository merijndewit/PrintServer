#include "ExternalStorage.h"
#include "Config.h"

namespace PrintServer
{
    static ExternalStorage* external_storage = nullptr;
    ExternalStorage::ExternalStorage()
    {
        esp_err_t ret;

            esp_vfs_fat_sdmmc_mount_config_t mount_config = 
            {
                .format_if_mount_failed = false,
                .max_files = 5,
                .allocation_unit_size = 16 * 1024
            };
            const char mount_point[] = MOUNT_POINT;
            ESP_LOGI(DEBUG_NAME, "Initializing SD card");

            ESP_LOGI(DEBUG_NAME, "Using SPI peripheral");

            host = SDSPI_HOST_DEFAULT();

            spi_bus_config_t bus_cfg = 
            {
                .mosi_io_num = PIN_NUM_MOSI,
                .miso_io_num = PIN_NUM_MISO,
                .sclk_io_num = PIN_NUM_CLK,
                .quadwp_io_num = -1,
                .quadhd_io_num = -1,
                .max_transfer_sz = 8192,
            };
            ret = spi_bus_initialize((spi_host_device_t)(host.slot), &bus_cfg, SDSPI_DEFAULT_DMA);
            if (ret != ESP_OK) 
            {
                ESP_LOGE(DEBUG_NAME, "Failed to initialize bus.");
                return;
            }

            sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
            slot_config.gpio_cs = (gpio_num_t)PIN_NUM_CS;
            slot_config.host_id = (spi_host_device_t)(host.slot);

            ESP_LOGI(DEBUG_NAME, "Mounting filesystem");
            ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

            if (ret != ESP_OK) 
            {
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

    ExternalStorage::~ExternalStorage()
    {
        esp_vfs_fat_sdcard_unmount(MOUNT_POINT, card);
        ESP_LOGI(DEBUG_NAME, "Card unmounted");

        spi_bus_free((spi_host_device_t)(host.slot));
    }

    bool ExternalStorage::init()
    {
        external_storage = new ExternalStorage();
        return 1;
    }

    void ExternalStorage::shutdown()
    {

    }

    ExternalStorage &ExternalStorage::get_instance()
    {
        return *external_storage;
    }

    void ExternalStorage::open_file(const char *path)
    {
        ESP_LOGI(DEBUG_NAME, "Opening file %s", path);
        current_open_file = fopen(path, "w");
        if (current_open_file == NULL) 
        {
            ESP_LOGE(DEBUG_NAME, "Failed to open file for writing");
        }
    }

    void ExternalStorage::close_file()
    {
        fclose(current_open_file);
        current_open_file = nullptr;
    }

    int ExternalStorage::write_to_open_file(const char *data, int chars)
    {
        int written_chars = 0;
        
        if (false)
        { // a way to write to the file while checking each character
            for (int i = 0; i < chars; i++) 
            {
                if (data[i] < '~') 
                {
                    fputc(data[i], current_open_file);
                    written_chars++;
                }
            }
        }

        {
            written_chars = fwrite(data, sizeof(char), chars, current_open_file);
        }
        return written_chars;
    }

    esp_err_t ExternalStorage::print_file(const char *path)
    {
        ESP_LOGI(DEBUG_NAME, "Reading file %s", path);
        FILE *f = fopen(path, "r");
        if (f == NULL) 
        {
            ESP_LOGE(DEBUG_NAME, "Failed to open file for reading");
            return ESP_FAIL;
        }
        char line[SD_WRITE_MAX_CHAR_SIZE];
        fgets(line, sizeof(line), f);
        fclose(f);

        // strip newline
        char *pos = strchr(line, '\n');
        if (pos) 
        {
            *pos = '\0';
        }
        ESP_LOGI(DEBUG_NAME, "Read from file: '%s'", line);

        return ESP_OK;
    }

    void ExternalStorage::unmount() 
    {
        esp_vfs_fat_sdcard_unmount(MOUNT_POINT, card);
        ESP_LOGI(DEBUG_NAME, "Card unmounted");

        spi_bus_free((spi_host_device_t)(host.slot));
    };
}
