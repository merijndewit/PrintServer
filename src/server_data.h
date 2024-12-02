#pragma once
#include <cinttypes>

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
    };

    inline server_data_t server_data_struct;
}