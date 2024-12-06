#pragma once
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_log.h"
#include "string.h"
#include <iostream>
#include "../Config.h"
#include "SD_Device.h"

namespace PrintServer
{
    #define SAVE_FILE "server_data.bin"

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
        SD_Device sd_device;

        FILE* current_open_file = nullptr;
        float gb_size = 0;
        bool reserved_reading = false;
    };
}