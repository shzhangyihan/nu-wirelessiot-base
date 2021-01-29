// BLE Scanning app
//
// Receives BLE advertisements

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf52840dk.h"
#include "simple_ble.h"

// BLE configuration
// This is mostly irrelevant since we are scanning only
static simple_ble_config_t ble_config = {
    // BLE address is c0:98:e5:4e:00:02
    .platform_id = 0x4E,  // used as 4th octet in device BLE address
    .device_id =
        0x0002,  // used as the 5th and 6th octet in the device BLE address, you
                 // will need to change this for each device you have
    .adv_name = "CS397/497",  // irrelevant in this example
    .adv_interval = MSEC_TO_UNITS(
        1000,
        UNIT_0_625_MS),  // send a packet once per second (minimum is 20 ms)
    .min_conn_interval =
        MSEC_TO_UNITS(500, UNIT_1_25_MS),  // irrelevant if advertising only
    .max_conn_interval =
        MSEC_TO_UNITS(1000, UNIT_1_25_MS),  // irrelevant if advertising only
};
simple_ble_app_t* simple_ble_app;

// Callback handler for advertisement reception
void ble_evt_adv_report(ble_evt_t const* p_ble_evt) {
    // extract the fields we care about
    ble_gap_evt_adv_report_t const* adv_report =
        &(p_ble_evt->evt.gap_evt.params.adv_report);
    uint8_t const* ble_addr =
        adv_report->peer_addr.addr;  // array of 6 bytes of the address
    uint8_t* adv_buf =
        adv_report->data
            .p_data;  // array of up to 31 bytes of advertisement payload data
    uint16_t adv_len =
        adv_report->data.len;  // length of advertisement payload data

    if (ble_addr[2] == 0x4e) {
        // printf("\n==============================\n");
        // printf("Received an advertisement!\n");
        // printf("Parsing payload: ");
        // for (size_t i = 0; i < adv_len; i++) {
        //     printf("%x ", adv_buf[i]);
        // }
        // printf("\n");
        size_t offset = 0;
        while (offset < adv_len) {
            size_t length = adv_buf[offset];
            size_t type = adv_buf[offset + 1];
            switch (type) {
                case 0x1: {
                    // printf("Flags: %d\n", adv_buf[offset + 2]);
                    break;
                }
                case 0x9: {
                    printf("Complete Local Name: ");
                    for (size_t i = 0; i < length - 1; i++) {
                        printf("%c", adv_buf[offset + 2 + i]);
                    }
                    printf("\n");
                    break;
                }
                case 0xFF: {
                    // printf("Manufacturer Specific Data: \n");
                    // printf("Company Identifier Code: %c%c\n",
                    //        adv_buf[offset + 2], adv_buf[offset + 3]);
                    // printf("Data: \n");
                    for (size_t i = 2; i < length - 1; i++) {
                        printf("%c", adv_buf[offset + 2 + i]);
                    }
                    printf("\n");
                    break;
                }
                default: {
                    // printf("Unrecognized type %x Skipping!\n", type);
                    break;
                }
            }
            offset = offset + length + 1;
        }
        // printf("End of advertisement\n");
        // printf("==============================\n");
    }
}

int main(void) {
    // Setup BLE
    // Note: simple BLE is our own library. You can find it in
    // `nrf5x-base/lib/simple_ble/`
    simple_ble_app = simple_ble_init(&ble_config);
    advertising_stop();

    // Start scanning
    scanning_start();

    // go into low power mode
    while (1) {
        power_manage();
    }
}
