#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_SSID        CONFIG_ESP_WIFI_SSID
#define WIFI_PASS        CONFIG_ESP_WIFI_PASSWORD
#define WIFI_CHANNEL     CONFIG_ESP_WIFI_CHANNEL
#define MAX_STA_CONN     CONFIG_ESP_MAX_STA_CONN

void wifi_init_softap(void);

#ifdef __cplusplus
}
#endif
