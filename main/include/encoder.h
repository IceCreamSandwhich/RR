#pragma once

// #include "config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "nvs_flash.h"

typedef struct encoder_t {
    volatile int position;
    volatile int lastEncoding;
    gpio_num_t pin_a;
    gpio_num_t pin_b;
} encoder_t;


#define LEFT_ENCODER_A GPIO_NUM_8
#define LEFT_ENCODER_B GPIO_NUM_9 

// #define LEFT_ENCODER_A GPIO_NUM_11
// #define LEFT_ENCODER_B GPIO_NUM_12


#define CPR 12 * 250

// void IRAM_ATTR encoder_isr_handler(void *arg);
void init_encoder(encoder_t* encoder);
void encoder_task(void* pvParameter);
BaseType_t encoder_service(void);