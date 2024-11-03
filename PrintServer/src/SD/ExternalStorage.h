
#pragma once

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"

#include "../Config.h"

namespace PrintServer
{
    #define SD_WRITE_MAX_CHAR_SIZE 8192
    #define MOUNT_POINT "/sdcard"

    class ExternalStorage
    {
    public:
        ExternalStorage(const ExternalStorage&) = delete;
        ExternalStorage& operator=(const ExternalStorage&) = delete;

        static bool init();
        static void shutdown();
        static ExternalStorage& get_instance();

        void open_file(const char *path);
        void close_file();

        int write_to_open_file(const char *data, int chars);
        esp_err_t print_file(const char *path);
        void unmount();
    
    private:
        ExternalStorage();
        ~ExternalStorage();
    private:
        sdmmc_card_t* card;
        sdmmc_host_t host;
        FILE* current_open_file = nullptr;
    };
}