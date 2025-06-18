#pragma once

// #include "config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define ENCODER_A 5
#define ENCODER_B 6

void IRAM_ATTR encoder_isr_handler(void *arg);

void setup_encoder(void);
void encoder_task(void *arg);