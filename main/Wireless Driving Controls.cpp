//headers
#include "include/wirelessControl_website.h"
#include "webserver_service.h"
#include "drivetrain.hpp"

// Commands
#define CMD_STOP      0
#define CMD_FORWARD   1
#define CMD_BACKWARD  2
#define CMD_LEFT      4
#define CMD_RIGHT     8

// GPIO pins (adjust as needed)
#define L1_PIN GPIO_NUM_37  // A1
#define L2_PIN GPIO_NUM_36  // A0
#define R1_PIN GPIO_NUM_35  // A2
#define R2_PIN GPIO_NUM_0   // D3
#define EL1_PIN GPIO_NUM_2  // D4
#define EL2_PIN GPIO_NUM_14 // D5
#define ER1_PIN GPIO_NUM_13 // D7
#define ER2_PIN GPIO_NUM_15 // D8

// Motor and GPIO init + control
void init_gpio(void)
{
    gpio_config_t io_conf = {};
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL<<L1_PIN)|(1ULL<<L2_PIN)|
                          (1ULL<<R1_PIN)|(1ULL<<R2_PIN);
    gpio_config(&io_conf);
    motor_stop();
}

void init_encoder(pcnt_unit_t unit, gpio_num_t gpio_a, gpio_num_t gpio_b)
{
    pcnt_config_t pcnt_config = {};
    pcnt_config.pulse_gpio_num = gpio_a;
    pcnt_config.ctrl_gpio_num  = gpio_b;
    pcnt_config.channel         = PCNT_CHANNEL_0;
    pcnt_config.unit            = unit;
    pcnt_config.pos_mode        = PCNT_COUNT_INC;
    pcnt_config.neg_mode        = PCNT_COUNT_DEC;
    pcnt_config.lctrl_mode      = PCNT_MODE_KEEP;
    pcnt_config.hctrl_mode      = PCNT_MODE_KEEP;
    pcnt_config.counter_h_lim   = 10000;
    pcnt_config.counter_l_lim   = -10000;
    pcnt_unit_config(&pcnt_config);
    pcnt_counter_pause(unit);
    pcnt_counter_clear(unit);
    pcnt_counter_resume(unit);
}

void motor_forward(void)
{
    gpio_set_level(L1_PIN, 1);
    gpio_set_level(L2_PIN, 0);
    gpio_set_level(R1_PIN, 0);
    gpio_set_level(R2_PIN, 1);
}
void motor_backward(void)
{
    gpio_set_level(L1_PIN, 0);
    gpio_set_level(L2_PIN, 1);
    gpio_set_level(R1_PIN, 1);
    gpio_set_level(R2_PIN, 0);
}
void motor_left(void)
{
    gpio_set_level(L1_PIN, 1);
    gpio_set_level(L2_PIN, 0);
    gpio_set_level(R1_PIN, 1);
    gpio_set_level(R2_PIN, 0);
}
void motor_right(void)
{
    gpio_set_level(L1_PIN, 0);
    gpio_set_level(L2_PIN, 1);
    gpio_set_level(R1_PIN, 0);
    gpio_set_level(R2_PIN, 1);
}
void motor_stop(void)
{
    gpio_set_level(L1_PIN, 0);
    gpio_set_level(L2_PIN, 0);
    gpio_set_level(R1_PIN, 0);
    gpio_set_level(R2_PIN, 0);
}