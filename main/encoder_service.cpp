#include "include/encoder.h"


typedef struct encoder_t {
    volatile int position;
    volatile int lastEncoding;
    gpio_num_t pin_a;
    gpio_num_t pin_b;
} encoder_t;

static encoder_t left_encoder = {0, 0b00, (gpio_num_t)0, (gpio_num_t)0};
static encoder_t right_encoder = {0, 0b00, (gpio_num_t)0,(gpio_num_t)0};

QueueHandle_t encoder_queue;

void init_encoder(int is_left, gpio_num_t pin_a, gpio_num_t pin_b)
{
    encoder_t encoder = {0, 0b00, pin_a, pin_b};
    if (is_left)
    {
        encoder = left_encoder;
    }
    else
    {
        encoder = right_encoder;
    }
    encoder.pin_a = pin_a;
    encoder.pin_b = pin_b;
    encoder.position = 0;
    encoder.lastEncoding = 0b00;

    // Set the encoder pins from the configuration
    gpio_set_direction((gpio_num_t)pin_a, GPIO_MODE_INPUT);
    gpio_set_direction((gpio_num_t)pin_b, GPIO_MODE_INPUT);
    // Set up the encoder pins
    gpio_config_t ioconf;
    ioconf.intr_type = GPIO_INTR_ANYEDGE;
    ioconf.mode = GPIO_MODE_INPUT;
    ioconf.pin_bit_mask = (1ULL << pin_a) | (1ULL << pin_b);
    ioconf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    ioconf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&ioconf);

    // Set up the interrupt
    gpio_install_isr_service(0);
    gpio_isr_handler_add(pin_a, encoder_isr_handler, (void *)&encoder);
    gpio_isr_handler_add(pin_b, encoder_isr_handler, (void *)&encoder);

    // Initialize the queue
    encoder_queue = xQueueCreate(10, sizeof(int));
    if (encoder_queue == NULL)
    {
        printf("Failed to create encoder queue\n");
        return;
    }
}

void IRAM_ATTR encoder_isr_handler(void *arg)
{
    encoder_t *encoder = (encoder_t *)arg;
    gpio_num_t pin_a = encoder->pin_a;
    gpio_num_t pin_b = encoder->pin_b;
    int lastEncoding = encoder->lastEncoding;
    int position = encoder->position;
    int a_val = gpio_get_level(pin_a);
    int b_val = gpio_get_level(pin_b);
    int encoding = (a_val << 1) | b_val;

    if (encoding == 0b00)
    {
        if (lastEncoding == 0b01)
        {
            position++;
        }
        else if (lastEncoding == 0b10)
        {
            position--;
        }
    }
    else if (encoding == 0b11)
    {
        if (lastEncoding == 0b10)
        {
            position++;
        }
        else if (lastEncoding == 0b01)
        {
            position--;
        }
    }

    xQueueSendFromISR(encoder_queue, &position, NULL);
}

void encoder_task(void *arg)
{
    int position;
    while (1)
    {
        // xQueueReceive(encoder_queue, &position, portMAX_DELAY);
        position = left_encoder.position; // For testing, use left encoder position
        printf("Encoder Position: %d\n", position);
    }
}