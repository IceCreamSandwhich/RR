/*
// Generic header files for OS and C
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"

// Application specific header files
#include "driver/twai.h"

// Custom header files
#define SEED_S3
#include "include/pins.h"

// Application specific definitions
#define TWAI_TX D0
#define TWAI_RX
#define TAG "main"
#define NO_MSGS 10
#define NO_ITERS 1

//Static variables

static SemaphoreHandle_t tx_sem;
static SemaphoreHandle_t rx_sem;
static SemaphoreHandle_t cntrl_sem;
static SemaphoreHandle_t done_sem;

static void create_msg(twai_message_t *msg, uint32_t id, uint8_t data)
{
    msg->extd = 0;
    msg->rtr = 0;
    msg->ss = 0;
    msg->self = 1;
    msg->dlc_non_comp = 0;
    msg->identifier = id;
    msg->data_length_code = 1;
    msg->data[0] = data;
}

static void transmit_task(void *arg)
{
    twai_message_t tx_msg = {
        .extd = 0,         // Standard Message Format w/ 11-bit ID
        .rtr = 0,          // Data Frame
        .ss = 0,           // Not single shot
        .self = 1,         // Self reception enabled
        .dlc_non_comp = 0, // DLC is 4-bit
        // ID and Payload
        .identifier = 0x123,
        .data_length_code = 1,
        .data = {0}};

    for (int i = 0; i < NO_ITERS; i++)
    {
        ESP_LOGI("Transmit Task", "Iteration %d", i);
        xSemaphoreTake(tx_sem, portMAX_DELAY);
        ESP_LOGI("Transmit Task", "Semaphore taken");
        for (int j = 0; j < NO_MSGS; j++)
        {
            ESP_LOGI("Transmit Task", "Transmitting message %d", j);
            tx_msg.data[0] = j;
            ESP_ERROR_CHECK(twai_transmit(&tx_msg, portMAX_DELAY));
            uint32_t alerts;
            twai_read_alerts(&alerts, 10);
            ESP_LOGI("Transmit Task", "Alerts: 0x%lx", alerts);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }

    vTaskDelete(NULL);
}

static void receive_task(void *arg)
{
    ESP_LOGI("Receive Task", "Task Started");
    twai_message_t rx_message;
    for (int iter = 0; iter < NO_ITERS; iter++)
    {
        ESP_LOGI("Receive Task", "Iteraction %d", iter);
        xSemaphoreTake(rx_sem, portMAX_DELAY);
        ESP_LOGI("Receive Task", "Semaphore taken");
        for (int i = 0; i < NO_MSGS; i++)
        {
            // Receive message and print message data
            ESP_LOGI("Receive Task", "Waiting for message %d", i);
            ESP_ERROR_CHECK(twai_receive(&rx_message, portMAX_DELAY));
            ESP_LOGI(TAG, "Msg received\tID 0x%lx\tData = %d", rx_message.identifier, rx_message.data[0]);
        }
        // Indicate to control task all messages received for this iteration
        xSemaphoreGive(cntrl_sem);
    }
    vTaskDelete(NULL);
}

static void control_task(void *arg)
{
    xSemaphoreTake(cntrl_sem, portMAX_DELAY);
    for (int iter = 0; iter < NO_ITERS; iter++)
    {
        ESP_ERROR_CHECK(twai_start());
        ESP_LOGI(TAG, "CAN Started");

        xSemaphoreGive(rx_sem);
        xSemaphoreGive(tx_sem);
        // Wait for all messages to be received
        xSemaphoreTake(cntrl_sem, portMAX_DELAY);

        ESP_ERROR_CHECK(twai_stop());
        ESP_LOGI(TAG, "CAN Stopped");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    xSemaphoreGive(done_sem);
    vTaskDelete(NULL);
}

void app_main(void)
{
    // Initialize TWAI
    ESP_ERROR_CHECK(twai_driver_install(&gen_config, &timing_config, &filter_config));
    ESP_LOGI(TAG, "TWAI Driver Installed");
    // Enabling alerts
    uint32_t alerts_to_enable = TWAI_ALERT_BUS_OFF | TWAI_ALERT_ERR_ACTIVE | TWAI_ALERT_TX_IDLE | TWAI_ALERT_TX_SUCCESS | TWAI_ALERT_TX_FAILED;
    ESP_ERROR_CHECK(twai_reconfigure_alerts(alerts_to_enable, NULL));

    // Create tasks and semaphores
    tx_sem = xSemaphoreCreateBinary();
    rx_sem = xSemaphoreCreateBinary();
    cntrl_sem = xSemaphoreCreateBinary();
    done_sem = xSemaphoreCreateBinary();

    xTaskCreatePinnedToCore(control_task, "Control Task", 4096, NULL, 10, NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(receive_task, "Receive Task", 4096, NULL, 9, NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(transmit_task, "Transmit Task", 4096, NULL, 8, NULL, tskNO_AFFINITY);

    // Run tasks
    xSemaphoreGive(cntrl_sem);
    xSemaphoreTake(done_sem, portMAX_DELAY);

    ESP_ERROR_CHECK(twai_driver_uninstall());
    ESP_LOGI(TAG, "TWAI Driver Uninstalled");

    // Delete tasks and semaphores
    vSemaphoreDelete(tx_sem);
    vSemaphoreDelete(rx_sem);
    vSemaphoreDelete(cntrl_sem);
    vSemaphoreDelete(done_sem);
}
*/
