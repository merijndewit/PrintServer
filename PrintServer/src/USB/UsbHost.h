#pragma once
#include "usb/cdc_acm_host.h"

namespace PrintServer
{
    struct class_driver_control {
        uint32_t actions;
        uint8_t dev_addr;
        usb_host_client_handle_t client_hdl;
        usb_device_handle_t dev_hdl;
    };
    class UsbHost
    {
    public:
        UsbHost();
        void Update();
    private:


    };
}
