// Buttons app
//
// Use buttons and switches to control LEDs

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"

// Pin definitions
#include "nrf52840dk.h"

#define CHECK_BUTTON(X)                                 \
    if (nrf_gpio_pin_read(BUTTON##X)) {                 \
        nrf_gpio_pin_set(LED##X);                       \
        if (!button_state.button_##X) {                 \
            printf("Button " #X " pressed %lu times\n", \
                   ++button_count.button_##X);          \
            button_state.button_##X = true;             \
        }                                               \
    } else {                                            \
        nrf_gpio_pin_clear(LED##X);                     \
        button_state.button_##X = false;                \
    }

typedef struct {
    uint32_t button_1;
    uint32_t button_2;
    uint32_t button_3;
    uint32_t button_4;
} button_count_t;

typedef struct {
    bool button_1;
    bool button_2;
    bool button_3;
    bool button_4;
} button_state_t;

int main(void) {
    // Initialize.
    nrf_gpio_cfg_output(LED1);
    nrf_gpio_cfg_output(LED2);
    nrf_gpio_cfg_output(LED3);
    nrf_gpio_cfg_output(LED4);
    nrf_gpio_pin_set(LED1);
    nrf_gpio_pin_set(LED2);
    nrf_gpio_pin_set(LED3);
    nrf_gpio_pin_set(LED4);
    nrf_gpio_cfg_input(BUTTON1, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BUTTON2, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BUTTON3, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BUTTON4, NRF_GPIO_PIN_PULLUP);

    button_count_t button_count = (button_count_t){0, 0, 0, 0};
    button_state_t button_state = (button_state_t){0, 0, 0, 0};

    // Enter main loop.
    while (1) {
        CHECK_BUTTON(1);
        CHECK_BUTTON(2);
        CHECK_BUTTON(3);
        CHECK_BUTTON(4);
    }
}
