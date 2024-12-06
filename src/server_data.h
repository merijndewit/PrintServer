#pragma once
#include <cinttypes>
#include "esp_log.h"
#include <cstring>
namespace PrintServer
{
    struct sd_card_filename
    {
        char name[256];
    };

    struct saved_server_data_t
    {
        uint32_t seconds_online = 0;
        uint16_t total_completed_prints = 0;
        uint16_t total_prints_started = 0;
    };

    inline saved_server_data_t saved_server_data_struct;

    struct server_data_t
    {
        uint16_t sd_file_count = 0;
        uint16_t sd_size_gb = 0;
        uint16_t sd_speed_mhz = 0;
        uint16_t sd_card_detected = 0;

        int current_print_time = 0;
        bool printing = false;

        sd_card_filename filenames[32];

        bool add_file(const char* name)
        {
            for (int i = 0; i < 32; i++)
            {
                if (has_file[i] == false)
                {
                    strncpy(filenames[i].name, name, 192);
                    has_file[i] = true;
                    sd_file_count++;
                    ESP_LOGI("SD","  File: %s\n", filenames[i].name);
                    return true;
                }
            }
            return false;
        }

        bool remove_file(const char* name)
        {
            for (int i = 0; i < 32; i++)
            {
                if (strncmp(filenames[i].name, name, strlen(filenames[i].name)) == 0)
                {
                    sd_file_count--;
                    strcpy(filenames[i].name, ""); 
                    has_file[i] = false;
                    ESP_LOGI("SD","  File: %s\n", filenames[i].name);
                    return true;
                }
            }
            return false;
        }

        int next_file_index(int file_index)
        {
            int file_count = 0;
            for (int i = 0; i < 32; i++)
            {
                if (has_file[i])
                {
                    file_count++;
                }
                if (file_count - 1 == file_index)
                {
                    return i;
                }
            }
            return 0;
        }
    private:
        bool has_file[32] = { 0 };
    };

    inline server_data_t server_data_struct;
}