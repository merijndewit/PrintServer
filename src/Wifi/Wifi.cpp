#include "Wifi.h"
#include "Config.h"

namespace PrintServer
{
    static EventGroupHandle_t s_wifi_event_group;
    static Wifi* wifi = nullptr;

    bool Wifi::init()
    {
        wifi = new Wifi();
        return Wifi::get_instance().connected;
    }

    void Wifi::shutdown()
    {

    }
    
    Wifi::Wifi()
    {        
        connect_wifi();
    }

    Wifi::~Wifi()
    {

    }

    Wifi& Wifi::get_instance()
    {
        return *wifi;
    }

    void Wifi::connect_wifi(void)
    {
        s_wifi_event_group = xEventGroupCreate();

        ESP_ERROR_CHECK(esp_netif_init());

        ESP_ERROR_CHECK(esp_event_loop_create_default());
        esp_netif_create_default_wifi_sta();

        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));

        esp_event_handler_instance_t instance_any_id;
        esp_event_handler_instance_t instance_got_ip;
        
        ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &Wifi::wifi_event_handler, NULL, &instance_any_id));
        ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &Wifi::wifi_event_handler, NULL, &instance_got_ip));

        wifi_config_t wifi_config = 
        {
            .sta = 
            {
                .ssid = USER_WIFI_SSID,
                .password = USER_WIFI_PASS,
                .threshold = 
                {
                    .authmode = WIFI_AUTH_WPA2_PSK
                }
            },
        };
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start());

        ESP_LOGI(DEBUG_NAME, "wifi_init_sta finished.");

        EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

        if (bits & WIFI_CONNECTED_BIT)
        {
            ESP_LOGI(DEBUG_NAME, "connected to:%s", USER_WIFI_SSID);
            connected = true;
        }
        else if (bits & WIFI_FAIL_BIT)
        {
            ESP_LOGI(DEBUG_NAME, "Failed to connect to:%s", USER_WIFI_SSID);
        }
        else
        {
            ESP_LOGE(DEBUG_NAME, "unexpected error occured while connecting to the wifi");
        }
        vEventGroupDelete(s_wifi_event_group);
    }

    void Wifi::wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
    {
        if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
        {
            esp_wifi_connect();
        }
        else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
        {
            if (Wifi::get_instance().connection_retries < MAX_CONNECTION_RETRIES)
            {
                esp_wifi_connect();
                Wifi::get_instance().connection_retries++;
                ESP_LOGI(DEBUG_NAME, "retry to connect to the AP");
            }
            else
            {
                xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            }
            ESP_LOGI(DEBUG_NAME, "connect to the AP fail");
        }
        else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
        {
            ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
            ESP_LOGI(DEBUG_NAME, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
            xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        }
    };
};