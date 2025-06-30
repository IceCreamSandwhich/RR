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
#include "RadioLibCustomHAL.hpp"
#include "wifi_service.h"

static const constexpr char *TAG = "MAIN";

void initialise(rr_state_t state); 

encoder_t left_encoder = {0, 0b00, LEFT_ENCODER_A, LEFT_ENCODER_B};
//encoder_t right_encoder = {0, 0b00, RIGHT_ENCODER_A, RIGHT_ENCODER_B};


extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Starting app_main");

    // Creating events queue
    state.connected = false;
    state.twai_active = false;
    state.led_enabled = false;
    state.radio_enabled = false;
    state.encoder_enabled = true;
    state.imu_enabled = false;
    state.wifi_enabled = true;

    // // Initialising peripherals
    initialise(state);
    
    while (1)
    {
        // Wait for events to be added to the queue
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        // rr_os_event_handler();
        printf("Left Encoder Position: %f\n", (float)(left_encoder.position / CPR));
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
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

void initialise(rr_state_t state)
{
    // Start WiFi
    if (state.wifi_enabled){
        wifi_init_softap();
    }

    /* Not using Radio right now if (state.radio_enabled)
    { 
        initialise_radio();
    } 
    */
    
    if (state.imu_enabled)
    {
        init_imu();
        ESP_LOGI(
            TAG, "Starting IMU service"
        );
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
        gpio_install_isr_service(0);
        ESP_LOGI(TAG, "Encoder Service Starting");
        init_encoder(&left_encoder);
        //init_encoder(&right_encoder);
        //encoder_service();
    }
    
    // initialise_drivetrain();
    // launch_rr_os_service();
}