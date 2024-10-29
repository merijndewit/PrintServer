#include "Webserver.h"
#include "Wifi/Wifi.h"

namespace PrintServer
{
    esp_err_t send_web_page(httpd_req_t *req)
    {
        int response = httpd_resp_send(req, webpage, HTTPD_RESP_USE_STRLEN);
        return response;
    }

    esp_err_t get_req_handler(httpd_req_t *req)
    {
        return send_web_page(req);
    }

    httpd_uri_t uri_main = 
    {
        .uri = "/",
        .method = HTTP_GET,
        .handler = get_req_handler,
        .user_ctx = NULL
    };

    WebServer::WebServer()
    {
        httpd_config_t config = HTTPD_DEFAULT_CONFIG();
        
        if (httpd_start(&server, &config) == ESP_OK)
        {
            httpd_register_uri_handler(server, &uri_main);
        }
    }

    void WebServer::send_webpage(const char* webpage)
    {

    }
}
