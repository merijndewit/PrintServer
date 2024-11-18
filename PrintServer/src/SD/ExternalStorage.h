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

    #define SDMMC_SLOT_CONFIG() {\
        .clk = GPIO_NUM_6, \
        .cmd = GPIO_NUM_11, \
        .d0 = GPIO_NUM_7, \
        .d1 = GPIO_NUM_8, \
        .d2 = GPIO_NUM_9, \
        .d3 = GPIO_NUM_10, \
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

        static bool init();
        static void shutdown();
        static ExternalStorage& get_instance();

        void open_file(const char *path);
        void close_file();

        int write_to_open_file(const char *data, int chars);
        esp_err_t print_file(const char *path);
        void unmount();
        float get_size() { return gb_size; };
    private:
        ExternalStorage();
        ~ExternalStorage();
    private:
        sdmmc_card_t* card;
        sdmmc_host_t host;
        FILE* current_open_file = nullptr;
        float gb_size = 0;
    };
}