#pragma once
#include "driver/uart.h"
#include <functional>

namespace PrintServer
{
    class UART
    {
    public:
        UART();
        void Update();
        void send(const char *data, int len);
        static void uart_read_task(void *arg);
        void set_callback(std::function<void(const char* message, int len)> callback);
    private:
    };
}
