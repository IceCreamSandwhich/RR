// OS and C Headers
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// ESP-IDF Headers
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"

// Component Headers
#include "BNO08x.hpp"

// Project Headers
#include "include/pins.h"
#include "include/drivetrain.hpp"
#include "include/twai_service.h"
#include "include/uros_service.h"
#include "include/imu_service.h"
#include "include/encoder.h"
#include "include/led.h"
#include "include/events.h"
#include "include/RadioLibCustomHAL.hpp"
#include "include/wifi_service.h"
#include "include/webserver_service.h"
#include "include/wirelessDrive_website.h"

static const constexpr char *TAG = "MAIN";

void initialise(rr_state_t state); 
void test_drive_code();

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Starting app_main");

    // Creating events queue (Set to True when you want a service working)
    state.connected = false;
    state.twai_active = false;
    state.led_enabled = false;
    state.radio_enabled = false;
    state.wifi_enabled = true;
    state.encoder_enabled = true;
    state.imu_enabled = false;

    // Initialising peripherals
    initialise(state);
    //test_drive_code();
}

// ISR handler must not use non-ISR-safe functions like `gpio_get_level` unless GPIO is input-only and stable

/*id initialise_radio()

{
    CLK, MISO, MOSI, CS
    RadioLibCustomHAL hal = RadioLibCustomHAL(1, 2, 3, 5);
    // CS, G0, RST
    SX1278 radio = new Module(&hal, 5, 7, 4);
    int status = radio.begin();
    if (status == RADIOLIB_ERR_NONE)
    {
        ESP_LOGI("Radio", "Radio initialised successfully");
    }
    else {ESP_LOGE("Radio", "Radio failed to initialise");}

    radio.setFrequency(433.0);
    radio.setSpreadingFactor(12);
}
*/

void test_drive_code()
{
    while (1) {
        // 1. Stop
        ESP_LOGI(TAG, "Stopping");
        speed_callback(0, 0);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        // 2. Move forward at ~50% speed for 2 seconds
        ESP_LOGI(TAG, "Moving forward");
        speed_callback(-512, -512);  // Move both motors forward
        vTaskDelay(5000 / portTICK_PERIOD_MS);

        // 3. Move backward for 2 seconds
        ESP_LOGI(TAG, "Moving backward");
        speed_callback(512, 512);  // Reverse both motors
        vTaskDelay(5000 / portTICK_PERIOD_MS);

        // 4. Spin in place (left forward, right backward)
        ESP_LOGI(TAG, "Spinning");
        speed_callback(512, -512);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void initialise(rr_state_t state)
{
    // WiFi for esp as AP and webservers
    if (state.wifi_enabled){
        wifi_init_softap();
        init_ws();
    }

    /* Not using Radio right now if (state.radio_enabled)
    { 
        initialise_radio();
    } 
    */

    if (state.imu_enabled)
    {
        init_imu();
        imu_service();
        ESP_LOGI(TAG, "Imu service started");
    }
    
    if (state.led_enabled)
    {
        initialise_led();
        set_led_color(INDEPENDENT_COLOR);
        twai_interrupt_init();
    }

    if (state.encoder_enabled)
    {
        ESP_LOGI(TAG, "Encoder Service Starting");
        init_encoder(&left_encoder);
        init_encoder(&right_encoder);
        encoder_service();
    }
    
    initialise_drivetrain();
    // launch_rr_os_service();
}