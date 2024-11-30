#include "UART.h"
#include "driver/uart.h"
#include "../config.h"

namespace PrintServer
{
    static const char *TAG = "USB-CDC";

    void send(const char *data) 
    {
        int len = uart_write_bytes(UART_PORT_NUM, data, strlen(data));
        ESP_LOGI(TAG, "Sent %d bytes to printer: %s", len, data);
    }

    void read(void) 
    {
        uint8_t data[UART_BUF_SIZE];
        int len = uart_read_bytes(UART_PORT_NUM, data, sizeof(data), 100 / portTICK_PERIOD_MS);
        if (len > 0) {
            data[len] = '\0';  // Null-terminate the string
            ESP_LOGI(TAG, "Received %d bytes from printer: %s", len, data);
        }
    }

    UART::UART()
    {
        uart_config_t uart_config = {
            .baud_rate = UART_BAUD_RATE,
            .data_bits = UART_DATA_8,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        };

        ESP_LOGI(TAG, "Configuring UART...");
        uart_param_config(UART_PORT_NUM, &uart_config);
        uart_set_pin(UART_PORT_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        uart_driver_install(UART_PORT_NUM, UART_BUF_SIZE, 0, 0, NULL, 0);

        ESP_LOGI(TAG, "UART configured successfully");
    }

    void UART::Update()
    {
        
    }
}