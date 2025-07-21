#include "include/encoder.h"
#include "esp_log.h"
#include "rom/ets_sys.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <fcntl.h>    // for open(), O_WRONLY, O_CREAT, etc.
#include <unistd.h>   // for close(), write(), etc.
#include <stdio.h>
#include "esp_spiffs.h"
#include <string.h>
#include <math.h>
#include "esp_timer.h"


#define TAG "ENCODER_SERVICE"

QueueHandle_t encoder_queue;
char enc_buf[512];
int enc_buf_ret;


encoder_t left_encoder = {0, 0b00, LEFT_ENCODER_A, LEFT_ENCODER_B};
encoder_t right_encoder = {0, 0b00, RIGHT_ENCODER_A, RIGHT_ENCODER_B};


// ISR handler must not use non-ISR-safe functions like `gpio_get_level` unless GPIO is input-only and stable
void IRAM_ATTR encoder_isr_handler(void *arg)
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
           encoder->position = encoder->position + 1 ;
       }
       else if (encoder->lastEncoding == 0b10)
       {
           encoder->position = encoder->position - 1;
       }
   }
   else if (encoding == 0b11)
   {
       if (encoder->lastEncoding == 0b10)
       {
           encoder->position = encoder->position + 1;
       }
       else if (encoder->lastEncoding == 0b01)
       {
           encoder->position = encoder -> position - 1;
       }
   }


   encoder->lastEncoding = encoding;
}


void init_encoder(encoder_t* encoder)
{
   static bool isr_service_installed = false;
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
   gpio_install_isr_service(0);  // Only needs to be called once in app
   gpio_isr_handler_add(encoder->pin_a, encoder_isr_handler, (void *)encoder);
   gpio_isr_handler_add(encoder->pin_b, encoder_isr_handler, (void *)encoder);
}


void encoder_task(void* pvParameter)
{
   (void)pvParameter; //pvparameter is not being used. Cast to void to match definition in header
   while(1)
   {
       vTaskDelay(1000 / portTICK_PERIOD_MS);
       // ESP_LOGI("ENC", "Right Pos: %f", ((float)(right_encoder.position) / CPR));
       // ESP_LOGI("ENC", "Left Pos: %f", ((float)(left_encoder.position) / CPR));


       // clear buf
       enc_buf[0] = '\0';
       // get timestamp
       int64_t enc_time_ms = esp_timer_get_time() / 1000;
       enc_time_to_buf(enc_time_ms);
       enc_data_to_buf((float)(right_encoder.position / CPR));
       enc_data_to_buf((float)(right_encoder.position / CPR));
       size_t len = strlen(enc_buf);
       if ((enc_buf_ret = snprintf(enc_buf + len, sizeof(enc_buf) - len, "\n")) < 0) {
               ESP_LOGE(TAG, "Failed to write to buffer");
       }
       enc_buf_to_text();
       vTaskDelay(200 / portTICK_PERIOD_MS);
   }
}


BaseType_t encoder_service(void)
{
   BaseType_t status = xTaskCreate(
       encoder_task,
       "encoder_task",
       4096,
       NULL,
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


// converts time in ms to seconds and ms and writes these to the buffer
void enc_time_to_buf(int64_t time_ms) {
    int32_t sec = time_ms / 1000;
    int32_t ms = time_ms % 1000;
    // write to buf
    size_t len = strlen(enc_buf);
    // writing seconds
    if (sec < 10) { // want to write 000x seconds
        if ((enc_buf_ret = snprintf(enc_buf + len, sizeof(enc_buf) - len, "000%ld", sec)) < 0) {
            ESP_LOGE(TAG, "Failed to write to buffer");
        }
    }
    else if (sec >= 10 && sec < 100) { // want to write 00xx seconds
        len = strlen(enc_buf);
        if ((enc_buf_ret = snprintf(enc_buf + len, sizeof(enc_buf) - len, "00%ld", sec)) < 0) {
            ESP_LOGE(TAG, "Failed to write to buffer");
        }
    }
    else if (sec >= 100 && sec < 1000) { // want to write 0xxx seconds
        len = strlen(enc_buf);
        if ((enc_buf_ret = snprintf(enc_buf + len, sizeof(enc_buf) - len, "0%ld", sec)) < 0) {
            ESP_LOGE(TAG, "Failed to write to buffer");
        }
    }
    else { // xxxx seconds
        len = strlen(enc_buf);
        if ((enc_buf_ret = snprintf(enc_buf + len, sizeof(enc_buf) - len, "%ld", sec)) < 0) {
            ESP_LOGE(TAG, "Failed to write to buffer");
        }
    }

    // writing ms
    if (ms < 10) { // want to write 00x ms
        len = strlen(enc_buf);
        if ((enc_buf_ret = snprintf(enc_buf + len, sizeof(enc_buf) - len, "00%ld", ms)) < 0) {
            ESP_LOGE(TAG, "Failed to write to buffer");
        }
    }
    else if (ms >= 10 && ms < 100) { // want to write 0xx ms
        len = strlen(enc_buf);
        if ((enc_buf_ret = snprintf(enc_buf + len, sizeof(enc_buf) - len, "0%ld", ms)) < 0) {
            ESP_LOGE(TAG, "Failed to write to buffer");
        }
    }
    else { // xxx ms
        len = strlen(enc_buf);
        if ((enc_buf_ret = snprintf(enc_buf + len, sizeof(enc_buf) - len, "%ld", ms)) < 0) {
            ESP_LOGE(TAG, "Failed to write to buffer");
        }
    }
}

void enc_data_to_buf(float data) {
    float abs_data = fabs(data);
    size_t len = strlen(enc_buf);
    if (data < 0) { // negative
        if ((enc_buf_ret = snprintf(enc_buf + len, sizeof(enc_buf) - len, "0")) < 0) {
               ESP_LOGE(TAG, "Failed to write to buffer");
        }
    }
    else { // positive
        if ((enc_buf_ret = snprintf(enc_buf + len, sizeof(enc_buf) - len, "1")) < 0) {
               ESP_LOGE(TAG, "Failed to write to buffer");
        }
    }
    len = strlen(enc_buf);
    if ((enc_buf_ret = snprintf(enc_buf + len, sizeof(enc_buf) - len, "%f", abs_data)) < 0) {
               ESP_LOGE(TAG, "Failed to write to buffer");
    }
}

void enc_buf_to_text() {  
    // write from buf to file
    FILE *f = fopen("/storage/encoder_data.txt", "a");  // "a" to append
    if (f == NULL) {
    ESP_LOGE("FILE", "Failed to open file for writing");
    } 
    else {
        fwrite(enc_buf, 1, strlen(enc_buf), f);  // write the buffer
        fclose(f);
        ESP_LOGI("FILE", "Data written to file");
    }   
}