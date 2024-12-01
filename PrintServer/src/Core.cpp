#include "Core.h"
#include "driver/gpio.h"
#include "SD/ExternalStorage.h"
#include "server_data.h"
#define LED_PIN GPIO_NUM_38

namespace PrintServer
{
    const char* base_path = "/data";

    Core::Core() : 
    wifi_connection_successful(Wifi::init()), 
    sd_connection_successful(ExternalStorage::init()),  
    webserver_running(WebServer::init(base_path)), 
    uart(), 
    printer_communicator(uart, ExternalStorage::get_instance())
    {
        WebServer::get_instance().set_callback(std::bind(&Core::recieved_websocket_command, this, std::placeholders::_1, std::placeholders::_2));
        esp_rom_gpio_pad_select_gpio(LED_PIN);
        gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

        gpio_set_level(LED_PIN, 0);
    }

    void Core::update()
    {
        if (timer.get_time() > 10000)
        {
            saved_server_data_struct.seconds_online += 10;
            timer.reset();
            //uart.Update();

            led_state %= 2;
            gpio_set_level(LED_PIN, led_state);
            led_state++;
            ExternalStorage::get_instance().save_data();
            //unsigned char message[] = { "hi" };
            //webserver.SendMessageToClients(message);
        }
    }

    void Core::recieved_websocket_command(const char *command, int len)
    {
        ESP_LOGI("CORE", "Received message from client: %s", command);
        printer_communicator.print_file(command);
    }
}