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
} data_t;

static void espnow_send_cb(const uint8_t *mac, esp_now_send_status_t status);

// ESP-NOW send callback
static void espnow_send_cb(const uint8_t *mac, esp_now_send_status_t status) {
    ESP_LOGI(TAG, "Send status: %s", status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

// Initialize WiFi in Station mode
static void wifi_init(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "WiFi initialized in Station mode");
}

// Initialize ESP-NOW
static void espnow_init(void) {
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(espnow_send_cb));
    
    // Add peer
    esp_now_peer_info_t peer = {
        .channel = 0,
        .encrypt = false,
    };
    memcpy(peer.peer_addr, receiver_mac, 6);
    ESP_ERROR_CHECK(esp_now_add_peer(&peer));
    
    ESP_LOGI(TAG, "ESP-NOW initialized");
}

void app_main(void) {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Initialize WiFi and ESP-NOW
    wifi_init();
    espnow_init();
    
    // Transmit data periodically
    sensor_data_t data = {0};
    while (1) {
        data.sensor_id = 1;
        data.temperature = 25.5 + (esp_random() % 100) / 10.0;
        data.humidity = 60.0 + (esp_random() % 200) / 10.0;
        data.timestamp = xTaskGetTickCount();
        
        esp_err_t result = esp_now_send(receiver_mac, (uint8_t *)&data, sizeof(data));
        if (result == ESP_OK) {
            ESP_LOGI(TAG, "Sent: ID=%d, Temp=%.1f, Hum=%.1f", 
                     data.sensor_id, data.temperature, data.humidity);
        } else {
            ESP_LOGE(TAG, "Send failed");
        }
        
        vTaskDelay(pdMS_TO_TICKS(2000)); // Send every 2 seconds
    }
}