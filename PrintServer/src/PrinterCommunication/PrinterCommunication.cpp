#include "PrinterCommunication.h"
namespace PrintServer
{
    PrinterCommunication::PrinterCommunication(UART& uart, ExternalStorage& sd_card) : uart(uart), sd_card(sd_card)
    {
        uart.set_callback(std::bind(&PrinterCommunication::recieved_message, this, std::placeholders::_1, std::placeholders::_2));
    }

    void PrinterCommunication::print_file(const char *path)
    {
        sd_card.start_reading_file(path);
        recieved_ok();
    }

    void PrinterCommunication::recieved_ok()
    {
        char buffer[MAX_GCODE_LENGTH] = {};

        int len = sd_card.get_next_line(buffer);
        uart.send(buffer, len);

        can_send = false;
    }

    void PrinterCommunication::recieved_message(const char *message, int len)
    {
        ESP_LOGI("PC-R", "recieved: %s", message);
        if (strncmp(message, "ok", 2) == 0 || strncmp(message, "OK", 2) == 0)
        {
            can_send = true;
            recieved_ok();
        }
        
    }

    void PrinterCommunication::Update()
    {

        
    }
}