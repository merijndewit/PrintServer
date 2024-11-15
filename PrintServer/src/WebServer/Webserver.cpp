#include "Webserver.h"

#include "../webpage/index.h"
#include "SD/ExternalStorage.h"

#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"

#include "Timers/Timer.h"

namespace PrintServer
{
    static WebServer* webserver = nullptr;

    struct file_server_data* server_data = new file_server_data(); 

    void WebServer::add_client(int fd) 
    { 
        websocket_client_t *client = (websocket_client_t *)malloc(sizeof(websocket_client_t)); 
        client->fd = fd; 
        client->next = webserver->clients; 
        webserver->clients = client; 
    }

    void WebServer::remove_client(int fd) 
    { 
        websocket_client_t** cur = &webserver->clients; 
        while (*cur != NULL) 
        { 
            if ((*cur)->fd == fd) 
            { 
                websocket_client_t *to_delete = *cur; 
                *cur = (*cur)->next; 
                free(to_delete); 
                return; 
            } 
            cur = &(*cur)->next; 
        } 
    }

    esp_err_t WebServer::send_web_page(httpd_req_t *req)
    {
        int response = httpd_resp_send(req, (const char*)webpage_index_html, webpage_index_html_len);
        return response;
    }

    esp_err_t WebServer::get_req_handler(httpd_req_t *req)
    {
        return send_web_page(req);
    }

    const char* WebServer::get_path_from_uri(char *dest, const char *base_path, const char *uri, size_t destsize)
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


    esp_err_t WebServer::upload_post_handler(httpd_req_t *req)
    {
        char filepath[128];
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

        int remaining = req->content_len; 
        int total_length = req->content_len;

        char file_path[256 + 64]; // should probably make something to calculate the correct size
        strcpy(file_path, MOUNT_POINT);
        strcat(file_path, "/");
        filename += strlen(UPLOAD_PATH); // move pointer by the length of UPLOAD_PATH
        strcat(file_path, filename);

        ESP_LOGI(DEBUG_NAME, "Opening : %s...", file_path);

        ExternalStorage& sd_card = ExternalStorage::get_instance();
        sd_card.open_file(file_path);

        Timer timer;

        char buffer[32];

        while (remaining > 0) 
        {
            if ((received = httpd_req_recv(req, buf, MIN(remaining, SCRATCH_BUFSIZE))) <= 0) 
            {
                if (received == HTTPD_SOCK_ERR_TIMEOUT) 
                {
                    continue;
                }

                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to receive file");
                return ESP_FAIL;
            }

            sd_card.write_to_open_file(buf, received);

            remaining -= received;

            float progress_precentage = (abs(remaining / (float)total_length - 1) * 100);
            unsigned int buffer_length = sprintf(buffer, "10: %.2f%%", progress_precentage);
            
            webserver->SendMessageToClients((unsigned char*)buffer, buffer_length);
        }

        float time_taken = timer.get_time() / 1000.f;
        float file_size = req->content_len / 1000000.f;
        float upload_speed = file_size / time_taken;
        ESP_LOGI(DEBUG_NAME, "It took: %f seconds to transfer %f MBs resulting in a speed of %f MB/s", time_taken, file_size, upload_speed);

        sd_card.close_file();
        return ESP_OK;
    }

    esp_err_t WebServer::socket_handler(httpd_req_t *req)
    {
        esp_err_t error_return = ESP_OK;
        if (req->method == HTTP_GET) 
        { 
            add_client(httpd_req_to_sockfd(req));
            
            unsigned char payload[] = {"Websocket connected!"};

            //will send a websocket response to the connecting websocket client
            error_return = WebServer::SendMessageToClient(req, payload);

            char buffer[128];
            unsigned int buffer_length = sprintf(buffer, "1;%.2f GB", ExternalStorage::get_instance().get_size());

            //will send a websocket response to the connecting websocket client
            webserver->SendMessageToClients((unsigned char*)buffer, buffer_length);
        } 
        
        if (error_return != ESP_OK) 
        { 
            ESP_LOGE(DEBUG_NAME, "WebSocket recv frame failed: %d", error_return); 
            remove_client(httpd_req_to_sockfd(req)); 
        }
        
        return error_return; 
    }
    
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
            httpd_uri_t uri_main = 
            {
                .uri = "/",
                .method = HTTP_GET,
                .handler = get_req_handler,
                .user_ctx = NULL
            };

            httpd_uri_t file_upload = 
            {
                .uri       = UPLOAD_PATH"*",
                .method    = HTTP_POST,
                .handler   = upload_post_handler,
                .user_ctx  = server_data
            };

            httpd_uri_t socket = 
            { 
                .uri = "/ws", 
                .method = HTTP_GET, 
                .handler = WebServer::socket_handler, 
                .user_ctx = NULL,
                .is_websocket = true
            };

            httpd_register_uri_handler(server, &uri_main);
            httpd_register_uri_handler(server, &file_upload);
            httpd_register_uri_handler(server, &socket);
        }
    }

    bool WebServer::init(const char* base_path)
    {
        webserver = new WebServer(base_path);
        return true;
    }

    void WebServer::shutdown()
    {

    }

    //sill send a message to all connected clients
    void WebServer::SendMessageToClients(unsigned char* message, unsigned int buffer_length)
    {
        websocket_client_t *client = clients; 
        httpd_ws_frame_t ws_pkt; 
        memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t)); 
        ws_pkt.payload = (uint8_t *)message; 
        ws_pkt.len = (buffer_length == 0) ? strlen((const char*)message) : buffer_length; 
        ws_pkt.type = HTTPD_WS_TYPE_TEXT; 
        while (client != NULL)
        { 
            httpd_ws_send_frame_async(server, client->fd, &ws_pkt); 
            client = client->next; 
        }
    }

    esp_err_t WebServer::SendMessageToClient(httpd_req_t* req, unsigned char* message)
    {
        httpd_ws_frame_t ws_pkt; 
        memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t)); 

        ws_pkt.type = HTTPD_WS_TYPE_TEXT; 
        ws_pkt.payload = (uint8_t*)message; 
        ws_pkt.len = strlen((char *)ws_pkt.payload); 

        return httpd_ws_send_frame(req, &ws_pkt);
    }
}
