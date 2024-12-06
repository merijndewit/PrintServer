#pragma once
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"
#include "string.h"
#include <iostream>
#include "../Config.h"

namespace PrintServer
{
    #define SD_WRITE_MAX_CHAR_SIZE 8192
    #define MOUNT_POINT "/sdcard"
    #define SAVE_FILE "server_data.bin"

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

    class ExternalStorage
    {
    public:
        ExternalStorage(const ExternalStorage&) = delete;
        ExternalStorage& operator=(const ExternalStorage&) = delete;

        void save_data();

        static bool init();
        static void shutdown();
        static ExternalStorage& get_instance();

        void open_file(const char *path);
        void open_file_read(const char *path);
        void close_file();
        void delete_file(const char *path);

        int write_to_open_file(const char *data, int chars);
        esp_err_t print_file(const char *path);
        void unmount();
        float get_size() { return gb_size; };

        void start_reading_file(const char *path);
        void stop_reading_file(const char *path);
        int get_next_line(char* line_buffer);
    private:
        ExternalStorage();
        ~ExternalStorage();
    private:
        sdmmc_card_t* card;
        sdmmc_host_t host;
        FILE* current_open_file = nullptr;
        float gb_size = 0;
        bool reserved_reading = false;
    };
}