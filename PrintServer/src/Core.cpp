#include "Core.h"
#include "driver/gpio.h"

#define LED_PIN GPIO_NUM_22
namespace PrintServer
{
    Core::Core() : connection_successful(Wifi::init()), webserver()
    {
        esp_rom_gpio_pad_select_gpio(LED_PIN);
        gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

        gpio_set_level(LED_PIN, 0);
    }

    void Core::update()
    {
        if (timer.get_time() > 300)
        {
            timer.reset();

            led_state %= 2;
            gpio_set_level(LED_PIN, led_state);
            led_state++;
        }
    }
}