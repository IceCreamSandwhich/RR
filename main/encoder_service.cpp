#include "include/encoder.h"
#include "esp_log.h"
#include "rom/ets_sys.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "ENCODER_SERVICE"

QueueHandle_t encoder_queue;

// ISR handler must not use non-ISR-safe functions like `gpio_get_level` unless GPIO is input-only and stable
static void IRAM_ATTR encoder_isr_handler(void *arg)
{
    encoder_t *encoder = (encoder_t *)arg;
    
    int a_val = gpio_get_level(encoder->pin_a);
    int b_val = gpio_get_level(encoder->pin_b);
    int encoding = (a_val << 1) | b_val;

    // Determine direction based on last state
    if (encoding == 0b00)
    {
        if (encoder->lastEncoding == 0b01)
        {
            encoder->position++ ;
        }
        else if (encoder->lastEncoding == 0b10)
        {
            encoder->position--;
        }
    }
    else if (encoding == 0b11)
    {
        if (encoder->lastEncoding == 0b10)
        {
            encoder->position++;
        }
        else if (encoder->lastEncoding == 0b01)
        {
            encoder->position--;
        }
    }

    encoder->lastEncoding = encoding;
}

void init_encoder(encoder_t* encoder)
{
    ESP_LOGI(TAG, "Setting up pins %d and %d", encoder->pin_a, encoder->pin_b);

    // Configure input pins
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << encoder->pin_a) | (1ULL << encoder->pin_b);
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    // Initialize encoder state
    encoder->position = 0;
    encoder->lastEncoding = (gpio_get_level(encoder->pin_a) << 1) | gpio_get_level(encoder->pin_b);

    // Enable interrupt service 
    gpio_isr_handler_add(encoder->pin_a, encoder_isr_handler, (void *)encoder);
    gpio_isr_handler_add(encoder->pin_b, encoder_isr_handler, (void *)encoder);
}

void encoder_task(void* pvParameter)
{
    encoder_t *encoder = (encoder_t *)pvParameter;

    while (1)
    {
        ESP_LOGI(TAG, "Encoder Position: %d", encoder->position);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

BaseType_t encoder_service(encoder_t *encoder)
{
    BaseType_t status = xTaskCreate(
        encoder_task,
        "encoder_task",
        2048,
        (void *)encoder,
        5,
        NULL);

    if (status == pdPASS)
    {
        ESP_LOGI(TAG, "Encoder service started");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to start encoder service");
    }

    return status;
}
