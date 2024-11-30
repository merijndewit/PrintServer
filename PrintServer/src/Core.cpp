#include "Core.h"
#include "driver/gpio.h"
#include "SD/ExternalStorage.h"

#define LED_PIN GPIO_NUM_38

namespace PrintServer
{
    const char* base_path = "/data";
    Core::Core() : wifi_connection_successful(Wifi::init()), sd_connection_successful(ExternalStorage::init()),  webserver_running(WebServer::init(base_path))
    {
        esp_rom_gpio_pad_select_gpio(LED_PIN);
        gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

        gpio_set_level(LED_PIN, 0);
    }

    void Core::update()
    {
        usbHost.Update();

        if (timer.get_time() > 1000)
        {
            timer.reset();

            led_state %= 2;
            gpio_set_level(LED_PIN, led_state);
            led_state++;

            //unsigned char message[] = { "hi" };
            //webserver.SendMessageToClients(message);
        }
    }
}