#ifndef WIFI_SERVICE_H
#define WIFI_SERVICE_H

#include "esp_err.h"
#include "nvs_flash.h"

#ifdef __cplusplus
extern "C" {
#endif

// Pulls in project-specific Wi-Fi credentials from Kconfig you can change them with idf.py menuconfig -> WiFi Configuration
#define WIFI_SERVICE_SSID           CONFIG_ESP_WIFI_SSID
#define WIFI_SERVICE_PASS           CONFIG_ESP_WIFI_PASSWORD
#define WIFI_SERVICE_CHANNEL        CONFIG_ESP_WIFI_CHANNEL
#define WIFI_SERVICE_MAX_STA_CONN   CONFIG_ESP_MAX_STA_CONN

/**
 * @brief Initialize the ESP32 Wi-Fi in SoftAP mode.
 *
 * This sets up the access point using settings defined via `menuconfig`.
 */
void wifi_init_softap(void);

#ifdef __cplusplus
}
#endif

#endif // WIFI_SERVICE_H
