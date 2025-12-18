/*
    Description:    ESPNOW transmitter node

    Author: Alexander Bobkov

    Date Created:   Dec 17, 2025
    Date Updated:   Dec 17, 2025

    Compiled using esp-idf v5.4.1

    Updated:
*/

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"

static const char *TAG = "ESPNOW_TX";

// MAC address of receiver (change to your receiver's MAC)
static uint8_t receiver_mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Broadcast

// Define your data structure (transmit & receive structs must be identical)
typedef struct {
    int node_id;
    float sensor1;
    float sensor2;
    uint32_t timestamp;
} sensor_data_t;

void app_main(void)
{

}
