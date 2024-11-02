#include "Webserver.h"

#include "../webpage/index.h"
#include "SD/ExternalStorage.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"

namespace PrintServer
{
    #define MAX_FILE_SIZE  (200*1024)
    #define SCRATCH_BUFSIZE  8192
    #define MAX_FILE_SIZE_STR "200KB"


    struct file_server_data 
    {
        char base_path[ESP_VFS_PATH_MAX + 1];
        char scratch[SCRATCH_BUFSIZE];
    };

    struct file_server_data* server_data = new file_server_data();

    esp_err_t send_web_page(httpd_req_t *req)
    {
        int response = httpd_resp_send(req, (const char*)webpage_index_html, webpage_index_html_len);
        return response;
    }

    esp_err_t get_req_handler(httpd_req_t *req)
    {
        return send_web_page(req);
    }

    static const char* get_path_from_uri(char *dest, const char *base_path, const char *uri, size_t destsize)
    {
        size_t base_pathlen = strlen(base_path);
        size_t pathlen = strlen(uri);

        const char *quest = strchr(uri, '?');
        if (quest) 
        {
            pathlen = MIN(pathlen, quest - uri);
        }
        const char *hash = strchr(uri, '#');
        if (hash) 
        {
            pathlen = MIN(pathlen, hash - uri);
        }

        if (base_pathlen + pathlen + 1 > destsize) 
        {
            return NULL;
        }

        strcpy(dest, base_path);
        strlcpy(dest + base_pathlen, uri, pathlen + 1);

        return dest + base_pathlen;
    }


    static esp_err_t upload_post_handler(httpd_req_t *req)
    {
        char filepath[64];
        struct stat file_stat;

        file_server_data* a = static_cast<file_server_data*>(req->user_ctx);
        const char *filename = get_path_from_uri(filepath, (a->base_path), req->uri, sizeof(filepath));

        if (!filename) 
        {
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Filename too long");
            return ESP_FAIL;
        }

        if (filename[strlen(filename) - 1] == '/') 
        {
            ESP_LOGE(DEBUG_NAME, "Invalid filename : %s", filename);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Invalid filename");
            return ESP_FAIL;
        }

        if (stat(filepath, &file_stat) == 0) 
        {
            ESP_LOGE(DEBUG_NAME, "File already exists : %s", filepath);
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "File already exists");
            return ESP_FAIL;
        }

        if (req->content_len > MAX_FILE_SIZE) 
        {
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "File size must be less than " MAX_FILE_SIZE_STR "!");


            return ESP_FAIL;
        }

        ESP_LOGI(DEBUG_NAME, "Receiving file : %s...", filename);

        char *buf = ((file_server_data *)req->user_ctx)->scratch;
        int received = 0;

        int remaining = 0; 
        remaining = req->content_len;

        
        ExternalStorage::get_instance().open_file(MOUNT_POINT"/test.txt");
        while (remaining > 0) 
        {

            ESP_LOGI(DEBUG_NAME, "Remaining size : %d", remaining);

            if ((received = httpd_req_recv(req, buf, MIN(remaining, SCRATCH_BUFSIZE))) <= 0) 
            {
                if (received == HTTPD_SOCK_ERR_TIMEOUT) 
                {
                    continue;
                }

                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to receive file");
                return ESP_FAIL;
            }


            if (received && (received != ExternalStorage::get_instance().write_to_open_file(buf, received))) 
            {

                ESP_LOGE(DEBUG_NAME, "File write failed!");
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to write file to storage");
                return ESP_FAIL;
            }

            remaining -= received;
        }

        ExternalStorage::get_instance().close_file();

        httpd_resp_set_status(req, "303 See Other");
        httpd_resp_set_hdr(req, "Location", "/");
    #ifdef CONFIG_EXAMPLE_HTTPD_CONN_CLOSE_HEADER
        httpd_resp_set_hdr(req, "Connection", "close");
    #endif
        httpd_resp_sendstr(req, "File uploaded successfully");
        return ESP_OK;
    }

    httpd_uri_t uri_main = 
    {
        .uri = "/",
        .method = HTTP_GET,
        .handler = get_req_handler,
        .user_ctx = NULL
    };

    httpd_uri_t file_upload = 
    {
        .uri       = "/upload/*",
        .method    = HTTP_POST,
        .handler   = upload_post_handler,
        .user_ctx  = server_data
    };
    
    WebServer::WebServer(const char* base_path)
    {
        if (server_data) 
        {
            ESP_LOGE(DEBUG_NAME, "File server already started");
        }

        if (!server_data) 
        {
            ESP_LOGE(DEBUG_NAME, "Failed to allocate memory for server data");
        }
        strlcpy(server_data->base_path, base_path, sizeof(server_data->base_path));

        httpd_config_t config = HTTPD_DEFAULT_CONFIG();
        
        config.uri_match_fn = httpd_uri_match_wildcard;
        if (httpd_start(&server, &config) == ESP_OK)
        {
            httpd_register_uri_handler(server, &uri_main);
            httpd_register_uri_handler(server, &file_upload);
        }
    }
}
