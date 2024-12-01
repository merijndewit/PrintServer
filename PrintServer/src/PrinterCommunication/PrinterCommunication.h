#pragma once
#include "UART/UART.h"
#include "SD/ExternalStorage.h"
#include <functional>
namespace PrintServer
{
    class PrinterCommunication
    {
    public:
        PrinterCommunication(UART& uart, ExternalStorage& sd_card);
        void print_file(const char *path);
        void recieved_ok();
        void recieved_message(const char* message, int len);
        void Update();
    private:
        UART& uart;
        ExternalStorage& sd_card;
        bool can_send = false;
    };
}
