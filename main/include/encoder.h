#pragma once

// #include "config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define ENCODER_A 5
#define ENCODER_B 6

void IRAM_ATTR encoder_isr_handler(void *arg);
void init_encoder(int is_left, gpio_num_t pin_a, gpio_num_t pin_b);
void encoder_task();