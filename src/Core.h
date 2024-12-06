#pragma once
#include "WebServer/Webserver.h"
#include "Wifi/Wifi.h"
#include "Timers/Timer.h"
#include "PrinterCommunication/UART/UART.h"
#include "PrinterCommunication/PrinterCommunication.h"
#include "PrinterCommunication/USB/UsbHost.h"

namespace PrintServer
{
    class Core
    {
    public:
        Core();
        void update();
        bool is_running() { return running; }
        void recieved_websocket_command(const char* command, int len);
    private:
        bool running = true;
        bool wifi_connection_successful;
        bool sd_connection_successful;
        bool webserver_running;
        unsigned int led_state = 1;
    private:
        Timer timer;
        UART uart;
        PrinterCommunication printer_communicator;
        //UsbHost usb_host;
    };
};