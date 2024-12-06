#include "UART.h"
#include "driver/uart.h"
#include "../../Config.h"
#include "esp_log.h"
#include <cstring>

namespace PrintServer
{
    static const char *TAG = "USB-CDC";
    std::function<void (const char *message, int len)> callback = nullptr;

    void UART::send(const char *data, int len) 
    {
        len = uart_write_bytes(UART_PORT_NUM, data, len);
        ESP_LOGI(TAG, "Sent %d bytes to printer: %s", len, data);
    }

    void UART::uart_read_task(void *arg) 
    {
        uint8_t data[UART_BUF_SIZE]; // Buffer to store received data
        while (1) 
        {
            // Read bytes from UART, non-blocking
            int len = uart_read_bytes(UART_PORT_NUM, data, sizeof(data), 10 / portTICK_PERIOD_MS); // 100ms timeout
            if (len > 0) 
            {
                data[len] = '\0';  // Null-terminate the string for logging
                ESP_LOGI(TAG, "Received %d bytes from UART", len);
                if (callback != nullptr)
                {
                    callback((const char*)data, len);
                }
                
            } 
            else if (len == 0) 
            {
                // No data received within timeout
                ESP_LOGD(TAG, "No data received from UART.");
            } 
            else 
            {
                // Error in reading UART
                ESP_LOGE(TAG, "Error reading from UART, length = %d", len);
            }

            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }

    void UART::set_callback(std::function<void(const char *message, int len)> set_callback)
    {
        callback = set_callback;
    }

    UART::UART()
    {
        uart_config_t uart_config = {
            .baud_rate = UART_BAUD_RATE,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        };

        ESP_LOGI(TAG, "Configuring UART...");
        uart_param_config(UART_PORT_NUM, &uart_config);
        uart_set_pin(UART_PORT_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        uart_driver_install(UART_PORT_NUM, UART_BUF_SIZE, 0, 0, NULL, 0);
        ESP_LOGI(TAG, "UART configured successfully");

        xTaskCreate(uart_read_task, "uart_read_task", (4096*2), NULL, 10, NULL);
        
    }

    void UART::Update()
    {
    }
}