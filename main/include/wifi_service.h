#pragma once

#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"

#ifdef __cplusplus
extern "C" {
#endif

// Adjust as needed
#define WIFI_SSID       "YourSSID"
#define WIFI_PASS       "YourPassword"
#define MAX_RETRY       5

void wifi_init_sta(void);

#ifdef __cplusplus
}
#endif
