#include "ExternalStorage.h"
#include "Config.h"

#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"

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
            .allocation_unit_size = 16 * 1024,
        };
        const char mount_point[] = MOUNT_POINT;
        ESP_LOGI(DEBUG_NAME, "Initializing SD card");

        ESP_LOGI(DEBUG_NAME, "Using SPI peripheral");

        host = SDMMC_HOST_DEFAULT();
        host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;

        sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG();
        slot_config.width = 4;
        slot_config.clk = GPIO_NUM_14;
        slot_config.cmd = GPIO_NUM_15;
        slot_config.d0 = GPIO_NUM_2;
        slot_config.d1 = GPIO_NUM_4;
        slot_config.d2 = GPIO_NUM_12;
        slot_config.d3 = GPIO_NUM_13;

        ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);
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

        uint64_t capacity = (double)card->csd.capacity * card->csd.sector_size / (1024.0 * 1024.0 * 1024.0);
        gb_size = capacity;
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

    //Returns chars written
    int ExternalStorage::write_to_open_file(const char *data, int chars)
    {
        int written_chars = 0;
        written_chars = fwrite(data, sizeof(char), chars, current_open_file);
        
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
