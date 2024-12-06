#include "UsbHost.h"
#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "usb/usb_host.h"
#include "usb/cdc_acm_host.h"

#define DAEMON_TASK_PRIORITY    2
#define CLASS_TASK_PRIORITY     3

#define EXAMPLE_USB_HOST_PRIORITY   (20)
#define EXAMPLE_USB_DEVICE_VID      (0x303A)
#define EXAMPLE_USB_DEVICE_PID      (0x4001) // 0x303A:0x4001 (TinyUSB CDC device)
#define EXAMPLE_USB_DEVICE_DUAL_PID (0x4002) // 0x303A:0x4002 (TinyUSB Dual CDC device)
#define EXAMPLE_TX_STRING           ("M115\n")
#define EXAMPLE_TX_TIMEOUT_MS       (1000)

namespace PrintServer
{
    static const char *TAG = "USB-CDC";
    static SemaphoreHandle_t device_disconnected_sem;
    cdc_acm_dev_hdl_t cdc_dev = NULL;

    /**
     * @brief Data received callback
     *
     * @param[in] data     Pointer to received data
     * @param[in] data_len Length of received data in bytes
     * @param[in] arg      Argument we passed to the device open function
     * @return
     *   true:  We have processed the received data
     *   false: We expect more data
     */
    bool UsbHost::handle_rx(const uint8_t *data, size_t data_len, void *arg)
    {
        ESP_LOGI(TAG, "Data received: %s", data);
        ESP_LOG_BUFFER_HEXDUMP(TAG, data, data_len, ESP_LOG_INFO);

        size_t received_len = 0;  // To store the number of bytes received


        return true;
    }

    /**
     * @brief Device event callback
     *
     * Apart from handling device disconnection it doesn't do anything useful
     *
     * @param[in] event    Device event type and data
     * @param[in] user_ctx Argument we passed to the device open function
     */
    void UsbHost::handle_event(const cdc_acm_host_dev_event_data_t *event, void *user_ctx)
    {
        switch (event->type) {
        case CDC_ACM_HOST_ERROR:
            ESP_LOGE(TAG, "CDC-ACM error has occurred, err_no = %i", event->data.error);
            break;
        case CDC_ACM_HOST_DEVICE_DISCONNECTED:
            ESP_LOGI(TAG, "Device suddenly disconnected");
            ESP_ERROR_CHECK(cdc_acm_host_close(event->data.cdc_hdl));
            xSemaphoreGive(device_disconnected_sem);
            break;
        case CDC_ACM_HOST_SERIAL_STATE:
            ESP_LOGI(TAG, "Serial state notif 0x%04X", event->data.serial_state.val);
            break;
        case CDC_ACM_HOST_NETWORK_CONNECTION:
        default:
            ESP_LOGW(TAG, "Unsupported CDC event: %i", event->type);
            break;
        }
    }

    /**
     * @brief USB Host library handling task
     *
     * @param arg Unused
     */
    static void usb_lib_task(void *arg)
    {
        while (1) {
            // Start handling system events
            uint32_t event_flags;
            usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
            if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
                ESP_ERROR_CHECK(usb_host_device_free_all());
            }
            if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
                ESP_LOGI(TAG, "USB: All devices freed");
                // Continue handling USB events to allow device reconnection
            }
        }
    }

    UsbHost::UsbHost()
    {
        device_disconnected_sem = xSemaphoreCreateBinary();
        assert(device_disconnected_sem);

        // Install USB Host driver. Should only be called once in entire application
        ESP_LOGI(TAG, "Installing USB Host");
        const usb_host_config_t host_config = {
            .skip_phy_setup = false,
            .intr_flags = ESP_INTR_FLAG_LEVEL1,
        };
        ESP_ERROR_CHECK(usb_host_install(&host_config));

        // Create a task that will handle USB library events
        BaseType_t task_created = xTaskCreate(usb_lib_task, "usb_lib", 4096, xTaskGetCurrentTaskHandle(), EXAMPLE_USB_HOST_PRIORITY, NULL);
        assert(task_created == pdTRUE);

        ESP_LOGI(TAG, "Installing CDC-ACM driver");
        ESP_ERROR_CHECK(cdc_acm_host_install(NULL));
    }

    void UsbHost::Update()
    {
        if (cdc_dev == NULL)
        {
            // Open USB device from tusb_serial_device example example. Either single or dual port configuration.
            ESP_LOGI(TAG, "Opening CDC ACM device 0x%04X:0x%04X...", CDC_HOST_ANY_VID, CDC_HOST_ANY_PID);
            esp_err_t err = cdc_acm_host_open(CDC_HOST_ANY_VID, CDC_HOST_ANY_PID, 0, &dev_config, &cdc_dev);
            if (ESP_OK != err) {
                ESP_LOGI(TAG, "Opening CDC ACM device 0x%04X:0x%04X...", CDC_HOST_ANY_VID, EXAMPLE_USB_DEVICE_DUAL_PID);
                err = cdc_acm_host_open(CDC_HOST_ANY_VID, CDC_HOST_ANY_PID, 0, &dev_config, &cdc_dev);
                if (ESP_OK != err) {
                    ESP_LOGI(TAG, "Failed to open device");
                    return;
                }
            }

            cdc_acm_host_desc_print(cdc_dev);
        }
        


        //vTaskDelay(pdMS_TO_TICKS(1000));

        // Test sending and receiving: responses are handled in handle_rx callback
        //ESP_ERROR_CHECK(cdc_acm_host_data_tx_blocking(cdc_dev, (const uint8_t *)EXAMPLE_TX_STRING, strlen(EXAMPLE_TX_STRING), EXAMPLE_TX_TIMEOUT_MS));
        
        //vTaskDelay(pdMS_TO_TICKS(1000));

    }
}