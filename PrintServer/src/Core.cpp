#include "Core.h"
#include "driver/gpio.h"
#include "SD/ExternalStorage.h"

#define LED_PIN GPIO_NUM_22

namespace PrintServer
{
    Core::Core() : connection_successful(Wifi::init()), webserver()
    {
        esp_rom_gpio_pad_select_gpio(LED_PIN);
        gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

        gpio_set_level(LED_PIN, 0);

        ExternalStorage externalStorage;

        char data[EXAMPLE_MAX_CHAR_SIZE];
        const char* file_gcode = MOUNT_POINT"/gcode.txt";
        snprintf(data, EXAMPLE_MAX_CHAR_SIZE, "%s\n", "G1 X100 Y100");
        externalStorage.write_to_storage(file_gcode, data);

        externalStorage.print_file(file_gcode);
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