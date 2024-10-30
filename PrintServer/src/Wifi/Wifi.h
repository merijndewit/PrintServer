#pragma once
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"

namespace PrintServer
{
    #define WIFI_CONNECTED_BIT BIT0
    #define WIFI_FAIL_BIT BIT1
    
    class Wifi
    {
    public:
        Wifi(const Wifi&) = delete;
        Wifi& operator=(const Wifi&) = delete;

        static bool init();
        static void shutdown();

        static Wifi& get_instance();
    private:
        Wifi();
        ~Wifi();
        void connect_wifi(void);
        static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
        int connection_retries = 0;

        bool connected = false;
    };
}
