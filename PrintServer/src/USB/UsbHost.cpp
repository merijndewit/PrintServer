#include "UsbHost.h"
#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "usb/usb_host.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "driver/uart.h"

#define CLASS_DRIVER_ACTION_OPEN_DEV    0x01
#define CLASS_DRIVER_ACTION_TRANSFER    0x02
#define CLASS_DRIVER_ACTION_CLOSE_DEV   0x03

namespace PrintServer
{
    static const char *TAG = "USB-CDC";
    static void usb_device_task(void *arg) {
        while (1) {
            tud_task();  // Poll for USB events
            vTaskDelay(10 / portTICK_PERIOD_MS);  // Delay to prevent high CPU usage
        }
    }
    
    void tud_mount_cb(uint8_t rhport) {
        ESP_LOGI(TAG, "USB device mounted");
    }

    // Callback function when the device is unmounted
    void tud_umount_cb(uint8_t rhport) {
        ESP_LOGI(TAG, "USB device unmounted");
    }

    // CDC-ACM Control Interface Callback
    //void tud_cdc_acm_control_interface_cb(uint8_t itf_num, cdcacm_control_interface_desc_t *desc) {
    //    ESP_LOGI(TAG, "CDC-ACM Control Interface: %d", itf_num);
   // }

    // CDC-ACM Data Interface Callback (for receiving data)
    void tud_cdc_acm_data_interface_cb(uint8_t itf_num, uint8_t *buffer, uint32_t len) 
    {
        ESP_LOGI(TAG, "Received %i bytes from USB CDC", (int)len);
    }

    void usb_cdc_read_task(void *arg)
    {
        uint8_t buffer[64];  // Buffer to read data
        while (1) {
            //int len = tud_cdc_acm_read(buffer, sizeof(buffer));
            //if (len > 0) {
            //    ESP_LOGI("TinyUSB", "Received %d bytes: %.*s", len, len, buffer);
            //}
            vTaskDelay(10 / portTICK_PERIOD_MS);  // Poll every 10 ms
        }
    }

    UsbHost::UsbHost()
    {
        ESP_LOGI("TinyUSB", "Initializing TinyUSB Host");
        esp_err_t ret = tusb_init();
        if (ret != ESP_OK) {
            ESP_LOGE("TinyUSB", "TinyUSB initialization failed: %d", ret);
        }
        
        xTaskCreate(usb_device_task, "usb_device_task", 4096, NULL, 5, NULL);
        
        // Start reading data from USB CDC
        xTaskCreate(usb_cdc_read_task, "usb_cdc_read_task", 4096, NULL, 5, NULL);
        
        ESP_LOGI("TinyUSB", "USB Host initialized and monitoring devices");
    }

    void UsbHost::Update()
    {
        
    }

}