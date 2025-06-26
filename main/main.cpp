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

#define LEFT_ENCODER_A (gpio_num_t) 1
#define LEFT_ENCODER_B (gpio_num_t) 5
// #define RIGHT_ENCODER_A (gpio_num_t) 3
// #define RIGHT_ENCODER_B (gpio_num_t) 4

static const constexpr char *TAG = "Main";

void initialise(rr_state_t state); 
/*the correct contact information must be provided to avoid having to pay for a second delivery fee. 
And last mile delivery is provided by USPS/PCF/ONTRAC/T FORCE/UDS*/



extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Starting app_main");

    // Creating events queue
    // state.connected = true;
    // state.twai_active = false;
    state.imu_enabled = true;
    // state.led_enabled = true;
    state.encoder_enabled = false;

    // // Initialising peripherals
    initialise(state);
    
    while (1)
    {
        // Wait for events to be added to the queue
        // rr_os_event_handler();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

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
    /* if (state.radio_enabled)
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
    

    /* if (state.led_enabled)
     {
         initialise_led();
         set_led_color(INDEPENDENT_COLOR);
         twai_interrupt_init();
     }
    */

    if (state.encoder_enabled)
    {
        init_encoder(true, LEFT_ENCODER_A, LEFT_ENCODER_B);
        //init_encoder(false, RIGHT_ENCODER_A, RIGHT_ENCODER_B);
        encoder_task();
    }

    //initialise_drivetrain();
    launch_rr_os_service();
}