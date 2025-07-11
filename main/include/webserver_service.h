#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "esp_event.h"
#include "esp_http_server.h"

// Logging tag
#define WS_TAG "ws_echo_server"

// Structure to store async WebSocket response context
typedef struct {
    httpd_handle_t hd;
    int fd;
} async_resp_arg_t;

// Starts the WebSocket-enabled HTTP server
httpd_handle_t start_webserver(void);

// Stops the running HTTP server
esp_err_t stop_webserver(httpd_handle_t server);

// Called on network disconnect (stops server)
void disconnect_handler(void* arg, esp_event_base_t event_base,
                        int32_t event_id, void* event_data);

// Called on network connect (starts server)
void connect_handler(void* arg, esp_event_base_t event_base,
                     int32_t event_id, void* event_data);

// Convenience function to initialize WebSocket server directly
void init_ws(void);

#ifdef __cplusplus
}
#endif