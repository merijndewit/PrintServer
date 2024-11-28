#pragma once
#include "usb/cdc_acm_host.h"

namespace PrintServer
{
    class UsbHost
    {
    public:
        UsbHost();
        void Update();
    private:
        static bool handle_rx(const uint8_t *data, size_t data_len, void *arg);
        static void handle_event(const cdc_acm_host_dev_event_data_t *event, void *user_ctx);

        cdc_acm_dev_hdl_t cdc_dev = NULL;

        const cdc_acm_host_device_config_t dev_config = 
        {
            .connection_timeout_ms = 1000,
            .out_buffer_size = 512,
            .in_buffer_size = 512,
            .event_cb = UsbHost::handle_event,
            .data_cb = UsbHost::handle_rx,
            .user_arg = NULL,
        };
    };
}
