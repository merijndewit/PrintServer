#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"

#include "../Config.h"

namespace PrintServer
{
    #define EXAMPLE_MAX_CHAR_SIZE 64
    #define MOUNT_POINT "/sdcard"

    class ExternalStorage
    {
    public:
        ExternalStorage();
        ~ExternalStorage();
        esp_err_t write_to_storage(const char *path, char *data);
        esp_err_t print_file(const char *path);
    private:
        sdmmc_card_t* card;
        sdmmc_host_t host;
    };
}