#pragma once
#include "Config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include "esp_err.h"

#include "esp_vfs.h"
#include "esp_spiffs.h"

#include "esp_http_server.h"

namespace PrintServer
{
    class WebServer
    {
    public:
        WebServer(const char* base_path);
        void SendMessageToClients(unsigned char* message);
    private:
        httpd_handle_t server = NULL;
    };
}
