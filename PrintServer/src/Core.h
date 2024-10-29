#pragma once
#include "WebServer/Webserver.h"
#include "Wifi/Wifi.h"

namespace PrintServer
{
    class Core
    {
    public:
        Core();
        void update() {};
        bool is_running() { return running; }
    private:
        bool running = true;
        bool connection_successful;
        WebServer webserver;
    };
};