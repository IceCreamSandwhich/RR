//Websocket Code to work with websites for RR (WirelessControl and the website to grab data)
#include "include/webserver_service.h"
#include "include/wirelessDrive_website.h"
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "esp_netif.h"
#include "esp_eth.h"

#include <esp_http_server.h>
#include <cstring>  // For memset, strcmp
#include <cstdlib>  // For malloc, calloc, free

static const char *TAG = "ws_echo_server";

// Structure holding server handle and socket file descriptor
struct async_resp_arg {
    httpd_handle_t hd;
    int fd;
};

// Asynchronous WebSocket send function
static void ws_async_send(void *arg)
{
    static const char *data = "Async data";
    struct async_resp_arg *resp_arg = (struct async_resp_arg *)arg; // cast back to struct type
    httpd_handle_t hd = resp_arg->hd;                              
    int fd = resp_arg->fd;

    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t)); // creates and zeroes out websocket frame
    ws_pkt.payload = (uint8_t *)data;             // populates frame with string
    ws_pkt.len = strlen(data);                    // and marks it as a text frame
    ws_pkt.type = HTTPD_WS_TYPE_TEXT; 

    httpd_ws_send_frame_async(hd, fd, &ws_pkt);   // sends frame over websocket
    free(resp_arg);                               // frees memory
}

// Triggers an async message to be sent over WebSocket
esp_err_t trigger_async_send(httpd_handle_t handle, httpd_req_t *req)
{
    struct async_resp_arg *resp_arg = (struct async_resp_arg *)malloc(sizeof(struct async_resp_arg));
    if (resp_arg == NULL) {
        return ESP_ERR_NO_MEM;
    }

    resp_arg->hd = req->handle;
    resp_arg->fd = httpd_req_to_sockfd(req);
    esp_err_t ret = httpd_queue_work(handle, ws_async_send, resp_arg);

    if (ret != ESP_OK) {
        free(resp_arg);
    }

    return ret;
}

// Handler for accessing wireless driving webpage 
static esp_err_t index_get_handler(httpd_req_t *req)
{
    // httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, HTML_CONTENT, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static const httpd_uri_t index_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_get_handler,
    .user_ctx  = NULL
};

// Handler for accessing IMU data page
static esp_err_t imu_handler(httpd_req_t *req)
{
    FILE *f = fopen("/storage/IMU_data.txt", "r");
    if (!f) {
        ESP_LOGE("HTTP", "Failed to open IMU_data.txt for reading");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    char buf[256];
    size_t chunksize;
    httpd_resp_set_type(req, "text/plain");

    // Optional: forces browser to download instead of view
    // httpd_resp_set_hdr(req, "Content-Disposition", "attachment; filename=\"IMU_data.txt\"");

    while ((chunksize = fread(buf, 1, sizeof(buf), f)) > 0) {
        if (httpd_resp_send_chunk(req, buf, chunksize) != ESP_OK) {
            fclose(f);
            ESP_LOGE("HTTP", "Failed to send file chunk");
            return ESP_FAIL;
        }
    }

    fclose(f);
    httpd_resp_send_chunk(req, NULL, 0); // End of response
    return ESP_OK;
    
}

// Register URI for imu_data
static const httpd_uri_t imu_uri = {
    .uri       = "/imu",         // Access it via http://<esp_ip>/imu
    .method    = HTTP_GET,
    .handler   = imu_handler,
    .user_ctx  = NULL
};

// Handler for accessing encoder data page
static esp_err_t encoder_handler(httpd_req_t *req)
{
    FILE *f = fopen("/storage/encoder_data.txt", "r");
    if (!f) {
        ESP_LOGE("HTTP", "Failed to open encoder_data.txt for reading");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    char buf[256];
    size_t chunksize;
    httpd_resp_set_type(req, "text/plain");

    // Optional: forces browser to download instead of view
    // httpd_resp_set_hdr(req, "Content-Disposition", "attachment; filename=\"IMU_data.txt\"");

    while ((chunksize = fread(buf, 1, sizeof(buf), f)) > 0) {
        if (httpd_resp_send_chunk(req, buf, chunksize) != ESP_OK) {
            fclose(f);
            ESP_LOGE("HTTP", "Failed to send file chunk");
            return ESP_FAIL;
        }
    }

    fclose(f);
    httpd_resp_send_chunk(req, NULL, 0); // End of response
    return ESP_OK;
}

// Register URI for encoder data
static const httpd_uri_t encoder_uri = {
    .uri       = "/encoder",         // Access it via http://<esp_ip>/imu
    .method    = HTTP_GET,
    .handler   = encoder_handler,
    .user_ctx  = NULL
};


// WebSocket handler that echoes messages and supports async trigger
static esp_err_t echo_handler(httpd_req_t *req)
{
    if (req->method == HTTP_GET) {
        ESP_LOGI(TAG, "Handshake done, the new connection was opened");
        return ESP_OK;
    }

    // init frame to receive data (text)
    httpd_ws_frame_t ws_pkt;
    uint8_t *buf = NULL;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;

    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0); // check frame len
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
        return ret;
    }

    if (ws_pkt.len) {
        buf = (uint8_t *)calloc(1, ws_pkt.len + 1); // memory to store incoming message + '\0'
        if (buf == NULL) {
            ESP_LOGE(TAG, "Failed to calloc memory for buf");
            return ESP_ERR_NO_MEM;
        }

        ws_pkt.payload = buf;
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len); // read frame data into buf
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
            free(buf);
            return ret;
        }

        ESP_LOGI(TAG, "Got command: %s", ws_pkt.payload); // print command
        
        // Here you would process the command (0, 1, 2, 4, 8)
        // and take appropriate action in your application
    }

    // Echo back the received command to client
    ret = httpd_ws_send_frame(req, &ws_pkt);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", ret);
    }

    free(buf);
    return ret;
}

// WebSocket URI config
static const httpd_uri_t ws = {
    .uri = "/ws",
    .method = HTTP_GET,
    .handler = echo_handler,
    .user_ctx = NULL,
    .is_websocket = true,
    .handle_ws_control_frames = NULL,
    .supported_subprotocol = NULL
};

// Starts the WebSocket-enabled HTTP server
httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);

    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &ws);
        httpd_register_uri_handler(server, &index_uri);  // Register the HTML handler
        httpd_register_uri_handler(server, &imu_uri);    // IMU file download
        httpd_register_uri_handler(server, &encoder_uri);    // encoder file download
        return server;
    }

    ESP_LOGI(TAG, "Error Starting server!");
    return NULL;
}
// Stops the WebSocket-enabled HTTP server
esp_err_t stop_webserver(httpd_handle_t server)
{
    return httpd_stop(server);
}

// Handles network disconnection (when wifi disconnects)
void disconnect_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{
    httpd_handle_t *server = (httpd_handle_t *)arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        if (stop_webserver(*server) == ESP_OK) {
            *server = NULL;
        } else {
            ESP_LOGE(TAG, "Failed to stop http server");
        }
    }
}

// Handles network connection (when wifi connects)
void connect_handler(void *arg, esp_event_base_t event_base,
                     int32_t event_id, void *event_data)
{
    httpd_handle_t *server = (httpd_handle_t *)arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}

// Initializes WebSocket service
void init_ws(void)
{
    static httpd_handle_t server = NULL;
    server = start_webserver();
}