// BLE Service example app
//
// Creates a BLE environmental sensing service

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf52840dk.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "simple_ble.h"

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

static simple_ble_service_t environmental_sensing_service = {
    {.uuid128 = {0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10,
                 0x00, 0x00, 0x1A, 0x18, 0x00, 0x00}}};

/*******************************************************************************
 *   State for this application
 ******************************************************************************/
// Main application state
simple_ble_app_t* simple_ble_app;

static simple_ble_char_t temperature_char = {.uuid16 = 0x2A6E};
static simple_ble_char_t humidity_char = {.uuid16 = 0x2A6F};
static simple_ble_char_t true_wind_speed_char = {.uuid16 = 0x2A70};
static simple_ble_char_t true_wind_direction_char = {.uuid16 = 0x2A71};
static simple_ble_char_t UV_index_char = {.uuid16 = 0x2A76};

static int16_t temperature;
static int16_t humidity;
static int16_t true_wind_speed;
static int16_t true_wind_direction;
static uint8_t UV_index;

void ble_evt_write(ble_evt_t const* p_ble_evt) {
    printf("Got write to a characteristic!\n");
}

int main(void) {
    printf("Board started. Initializing BLE: \n");

    // Setup BLE
    simple_ble_app = simple_ble_init(&ble_config);

    // R = C * M * 10^d * 2^b
    // value taken from Evanston
    temperature = -1600;          // M = 1, d = -2, b = 0, R = -16.00 C
    humidity = 5400;              // M = 1, d = -2, b = 0, R = 54.00 %
    true_wind_speed = 268;        // M = 1, d = -2, b = 0, R = 2.68 m/s
    true_wind_direction = 29000;  // M = 1, d = -2, b = 0, R = 290.00 degree
    UV_index = 0;                 // Unitless

    simple_ble_add_service(&environmental_sensing_service);
    simple_ble_add_characteristic(
        1, 0, 0, 0, sizeof(temperature), (uint8_t*)&temperature,
        &environmental_sensing_service, &temperature_char);
    simple_ble_add_characteristic(
        1, 0, 0, 0, sizeof(humidity), (uint8_t*)&humidity,
        &environmental_sensing_service, &humidity_char);
    simple_ble_add_characteristic(
        1, 0, 0, 0, sizeof(true_wind_speed), (uint8_t*)&true_wind_speed,
        &environmental_sensing_service, &true_wind_speed_char);
    simple_ble_add_characteristic(
        1, 0, 0, 0, sizeof(true_wind_direction), (uint8_t*)&true_wind_direction,
        &environmental_sensing_service, &true_wind_direction_char);
    simple_ble_add_characteristic(
        1, 0, 0, 0, sizeof(UV_index), (uint8_t*)&UV_index,
        &environmental_sensing_service, &UV_index_char);
    // Start Advertising
    simple_ble_adv_only_name();

    while (1) {
        power_manage();
    }
}
