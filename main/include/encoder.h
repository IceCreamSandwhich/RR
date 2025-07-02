#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

typedef struct encoder_t {
    volatile int position;
    volatile int lastEncoding;
    gpio_num_t pin_a;
    gpio_num_t pin_b;
} encoder_t;


#define LEFT_ENCODER_A GPIO_NUM_6 
#define LEFT_ENCODER_B GPIO_NUM_5  

#define RIGHT_ENCODER_A GPIO_NUM_1 
#define RIGHT_ENCODER_B GPIO_NUM_2 


#define CPR (1375)   //(Manually Counted for one full rotation for our motors change as needed) 

// void IRAM_ATTR encoder_isr_handler(void *arg);
void init_encoder(encoder_t* encoder);
void encoder_task(void* pvParameter);
BaseType_t encoder_service(void);