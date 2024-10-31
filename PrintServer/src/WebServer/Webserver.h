#pragma once
#include "Config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_http_server.h"

namespace PrintServer
{
    class WebServer
    {
    public:
        WebServer();
    private:
        httpd_handle_t server = NULL;
    };
}
