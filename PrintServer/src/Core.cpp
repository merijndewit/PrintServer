#include "Core.h"
#include "driver/gpio.h"
#include "SD/ExternalStorage.h"

#define LED_PIN GPIO_NUM_22

namespace PrintServer
{
    const char* base_path = "/data";
    Core::Core() : wifi_connection_successful(Wifi::init()), sd_connection_successful(ExternalStorage::init()),  webserver(base_path)
    {
        esp_rom_gpio_pad_select_gpio(LED_PIN);
        gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

        gpio_set_level(LED_PIN, 0);

        //char data[SD_WRITE_MAX_CHAR_SIZE];
        //const char* file_gcode = MOUNT_POINT"/gcode.txt";
        //snprintf(data, SD_WRITE_MAX_CHAR_SIZE, "%s\n", "G1 X100 Y100");

        //ExternalStorage::get_instance().write_to_storage(file_gcode, data);

        //ExternalStorage::get_instance().print_file(file_gcode);
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