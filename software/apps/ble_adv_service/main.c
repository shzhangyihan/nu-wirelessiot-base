// BLE Service example app
//
// Creates a BLE service and blinks an LED

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "app_error.h"
#include "nrf.h"
#include "nrf52840dk.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "simple_ble.h"

#define BLINK_COUNTER_MAX 200000

// Intervals for advertising and connections
static simple_ble_config_t ble_config = {
    // c0:98:e5:4e:xx:xx
    .platform_id = 0x4E,  // used as 4th octect in device BLE address
    .device_id = 0xAABB,
    .adv_name = "CS397/497",  // used in advertisements if there is room
    .adv_interval = MSEC_TO_UNITS(1000, UNIT_0_625_MS),
    .min_conn_interval = MSEC_TO_UNITS(500, UNIT_1_25_MS),
    .max_conn_interval = MSEC_TO_UNITS(1000, UNIT_1_25_MS),
};

static simple_ble_service_t my_advanced_service = {
    {.uuid128 = {0x70, 0x6C, 0x98, 0x41, 0xCE, 0x43, 0x14, 0xA9, 0xB5, 0x4D,
                 0x22, 0x2B, 0x88, 0x10, 0xE6, 0x32}}};

// static simple_ble_char_t led_state_char = {.uuid16 = 0x1089};
// static bool led_state = false;

static simple_ble_char_t button_notify_char = {.uuid16 = 0x1089};
static simple_ble_char_t LED_blink_char = {.uuid16 = 0x1090};
static simple_ble_char_t print_char = {.uuid16 = 0x1091};

static uint8_t button_state = 0;
static uint8_t LED_id = 0;
static int print_val = 0;

/*******************************************************************************
 *   State for this application
 ******************************************************************************/
// Main application state
simple_ble_app_t* simple_ble_app;

void ble_evt_write(ble_evt_t const* p_ble_evt) {
    // printf("in evt\n");
    if (simple_ble_is_char_event(p_ble_evt, &print_char)) {
        printf("RX: %d\n", print_val);
    }
}

int main(void) {
    printf("Board started. Initializing BLE: \n");

    ret_code_t error_code = NRF_SUCCESS;

    // initialize RTT library
    error_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(error_code);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    nrf_gpio_cfg_output(LED1);
    nrf_gpio_cfg_output(LED2);
    nrf_gpio_cfg_output(LED3);
    nrf_gpio_cfg_output(LED4);
    nrf_gpio_cfg_input(BUTTON1, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BUTTON2, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BUTTON3, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BUTTON4, NRF_GPIO_PIN_PULLUP);

    // Setup BLE
    simple_ble_app = simple_ble_init(&ble_config);

    simple_ble_add_service(&my_advanced_service);

    simple_ble_add_characteristic(1, 0, 1, 0, sizeof(button_state),
                                  (uint8_t*)&button_state, &my_advanced_service,
                                  &button_notify_char);
    simple_ble_add_characteristic(1, 1, 0, 0, sizeof(LED_id), (uint8_t*)&LED_id,
                                  &my_advanced_service, &LED_blink_char);
    simple_ble_add_characteristic(1, 1, 0, 0, sizeof(print_val),
                                  (uint8_t*)&print_val, &my_advanced_service,
                                  &print_char);

    // Start Advertising
    simple_ble_adv_only_name();
    int blink_counter = 0;

    while (1) {
        uint8_t new_button_state;
        if (!nrf_gpio_pin_read(BUTTON1)) {
            new_button_state = 1;
        } else if (!nrf_gpio_pin_read(BUTTON2)) {
            new_button_state = 2;
        } else if (!nrf_gpio_pin_read(BUTTON3)) {
            new_button_state = 3;
        } else if (!nrf_gpio_pin_read(BUTTON4)) {
            new_button_state = 4;
        } else {
            new_button_state = 0;
        }
        if (new_button_state != button_state) {
            button_state = new_button_state;
            // printf("Notify connected device %d\n", button_state);
            simple_ble_notify_char(&button_notify_char);
            // printf("Done notify%d\n", button_state);
        }

        if (blink_counter < BLINK_COUNTER_MAX) {
            blink_counter++;
        } else {
            blink_counter = 0;
            if (LED_id == 1) {
                nrf_gpio_pin_toggle(LED1);
            } else {
                nrf_gpio_pin_set(LED1);
            }
            if (LED_id == 2) {
                nrf_gpio_pin_toggle(LED2);
            } else {
                nrf_gpio_pin_set(LED2);
            }
            if (LED_id == 3) {
                nrf_gpio_pin_toggle(LED3);
            } else {
                nrf_gpio_pin_set(LED3);
            }
            if (LED_id == 4) {
                nrf_gpio_pin_toggle(LED4);
            } else {
                nrf_gpio_pin_set(LED4);
            }
        }

        // printf("but %d LED %d\n", button_state, LED_id);
        // nrf_delay_ms(500);
    }
}
