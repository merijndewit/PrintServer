#pragma once
#include "WebServer/Webserver.h"
#include "Wifi/Wifi.h"
#include "Timers/Timer.h"
#include "USB/UsbHost.h"

namespace PrintServer
{
    class Core
    {
    public:
        Core();
        void update();
        bool is_running() { return running; }
    private:
        bool running = true;
        bool wifi_connection_successful;
        bool sd_connection_successful;
        bool webserver_running;
        unsigned int led_state = 1;
    private:
        Timer timer;
        UsbHost usbHost;
    };
};