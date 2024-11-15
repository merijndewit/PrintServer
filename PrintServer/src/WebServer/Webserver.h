#pragma once
#include "Config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include "esp_err.h"

#include "esp_vfs.h"
#include "esp_spiffs.h"

#include "esp_http_server.h"

namespace PrintServer
{
    #define MAX_FILE_SIZE  (20000*1024)
    #define SCRATCH_BUFSIZE 65536
    #define MAX_FILE_SIZE_STR "20000KB"
    #define UPLOAD_PATH "/upload/"

    struct file_server_data 
    {
        char base_path[ESP_VFS_PATH_MAX + 1];
        char scratch[SCRATCH_BUFSIZE];
    };

    typedef struct websocket_client 
    { 
        int fd; 
        struct websocket_client *next; 
    } websocket_client_t;
    
    class WebServer
    {
    public:
        WebServer(const WebServer&) = delete;
        WebServer& operator=(const WebServer&) = delete;

        static bool init(const char* base_path);
        static void shutdown();

        void SendMessageToClients(unsigned char* message, unsigned int buffer_length = 0);
        static esp_err_t SendMessageToClient(httpd_req_t* req, unsigned char* message);
    private:
        WebServer(const char* base_path);

        static void add_client(int fd);
        static void remove_client(int fd);
        static esp_err_t send_web_page(httpd_req_t* req);
        static esp_err_t get_req_handler(httpd_req_t* req);
        static const char* get_path_from_uri(char* dest, const char* base_path, const char* uri, size_t destsize);
        static esp_err_t socket_handler(httpd_req_t* req);
        static esp_err_t upload_post_handler(httpd_req_t* req);
    private:
        httpd_handle_t server = NULL;
        websocket_client_t* clients = NULL; 
    };
}
