/*
    Description:    ESPNOW receiver node

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
#include "freertos/queue.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"

static const char *TAG = "ESPNOW_RX";

// Define matching data structure (must match transmitter)
typedef struct {
    int node_id;
    float sensor1;
    float sensor2;
    uint32_t timestamp;
} data_t;

// Queue to pass received data from callback to task
static QueueHandle_t espnow_queue;

// Structure for queue items
typedef struct {
    uint8_t mac[6];
    data_t data;
    int data_len;
} espnow_event_t;

// ESP-NOW receive callback
static void espnow_recv_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len) {
    if (len == sizeof(data_t)) {
        espnow_event_t evt;
        memcpy(evt.mac, recv_info->src_addr, 6);
        memcpy(&evt.data, data, sizeof(data_t));
        evt.data_len = len;
        
        // Send to queue for processing in task
        if (xQueueSend(espnow_queue, &evt, 0) != pdTRUE) {
            ESP_LOGW(TAG, "Queue full, dropping packet");
        }
    } else {
        ESP_LOGW(TAG, "Received data with wrong size: %d bytes", len);
    }
}

// Initialize WiFi in Station mode
static void wifi_init(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    // Print MAC address
    uint8_t mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, mac);
    ESP_LOGI(TAG, "WiFi initialized. MAC: %02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

// Initialize ESP-NOW
static void espnow_init(void) {
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_recv_cb(espnow_recv_cb));
    ESP_LOGI(TAG, "ESP-NOW initialized");
}

// RTOS task for processing received data
static void espnow_recv_task(void *pvParameter) {
    espnow_event_t evt;
    
    ESP_LOGI(TAG, "ESP-NOW receive task started");
    
    while (1) {
        // Wait for data from queue
        if (xQueueReceive(espnow_queue, &evt, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI(TAG, "Received from %02X:%02X:%02X:%02X:%02X:%02X",
                     evt.mac[0], evt.mac[1], evt.mac[2], 
                     evt.mac[3], evt.mac[4], evt.mac[5]);
            
            ESP_LOGI(TAG, "  Node ID: %d", evt.data.node_id);
            ESP_LOGI(TAG, "  Timestamp: %lu", evt.data.timestamp);
            
            // Add your data processing logic here
            // For example: store to database, forward to cloud, etc.
        }
    }
}

void app_main(void) {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Create queue for ESP-NOW events
    espnow_queue = xQueueCreate(10, sizeof(espnow_event_t));
    if (espnow_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create queue");
        return;
    }
    
    // Initialize WiFi and ESP-NOW
    wifi_init();
    espnow_init();
    
    // Create ESP-NOW receive task
    xTaskCreate(espnow_recv_task, "espnow_recv", 4096, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "Initialization complete, waiting for data...");
}