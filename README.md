# ESP-NOW Wireless Communication for ESP32

A minimal, production-ready implementation of ESP-NOW wireless communication using ESP-IDF v5.4.1 with FreeRTOS task architecture.

## Overview

This repository contains two programs that demonstrate bidirectional struct-based data transmission between ESP32 devices using Espressif ESP-NOW protocol. ESP-NOW provides low-latency, connectionless communication without requiring WiFi router infrastructure.

## Features

  - Struct-based data transmission: type-safe communication using custom data structures
  - Broadcast or direct transmission: support for both peer-to-peer and broadcast modes
  - MAC address validation: automatic sender identification and packet validation
  - Production-ready error handling: comprehensive logging and error reporting

## Repository Structure

esp-now-communication/
├── transmitter/
│   ├── main/
│   │   └── main.c          # ESP-NOW transmitter with RTOS task
│   └── CMakeLists.txt
├── receiver/
│   ├── main/
│   │   └── main.c          # ESP-NOW receiver with RTOS task
│   └── CMakeLists.txt
└── README.md

## Hardware Requirements

2x ESP32 development boards (ESP32, ESP32-S2, ESP32-S3, or ESP32-C3)
USB cables for programming and power

## Software Requirements

ESP-IDF v5.4.1 or later
Python 3.x (for ESP-IDF tools)

## Quick Start

  1. Clone and Setup
    bashgit clone <repository-url>
    cd esp-now-communication
    . $IDF_PATH/export.sh  # Setup ESP-IDF environment

  2. Flash Receiver
bashcd receiver
idf.py set-target esp32  # or esp32s3, esp32c3, etc.
idf.py build flash monitor
Important: Note the MAC address printed in the monitor output.
3. Update Transmitter MAC Address
Edit transmitter/main/main.c and replace the broadcast MAC address:
cstatic uint8_t receiver_mac[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}; // Your receiver's MAC
4. Flash Transmitter
bashcd ../transmitter
idf.py set-target esp32
idf.py build flash monitor

## Data Structure

The example transmits sensor data with the following structure:

ctypedef struct {
    int sensor_id;
    float temperature;
    float humidity;
    uint32_t timestamp;
} data_t;

Customize this structure for your specific application needs.

## Configuration

Transmission Rate
Adjust the delay in espnow_send_task():
cvTaskDelay(pdMS_TO_TICKS(2000)); // Send every 2 seconds
Queue Size
Modify queue depth in receiver for higher data rates:
cespnow_queue = xQueueCreate(10, sizeof(espnow_event_t)); // 10 items
Task Priority
Change task priorities based on your system requirements:
cxTaskCreate(espnow_send_task, "espnow_send", 4096, NULL, 5, NULL); // Priority 5
Communication Modes
Broadcast Mode (Default)
cstatic uint8_t receiver_mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
Sends to all nearby ESP-NOW devices.
Direct Mode (Recommended)
cstatic uint8_t receiver_mac[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
Sends to a specific device by MAC address.

## Architecture

### Transmitter Flow

Initialize WiFi in Station mode
Initialize ESP-NOW and add peer
Create RTOS task for periodic transmission
Send callback confirms delivery status

### Receiver Flow

Initialize WiFi in Station mode
Initialize ESP-NOW with receive callback
Create queue for ISR-to-task communication
Receive callback validates and queues data
RTOS task processes data from queue

## Performance

Latency: < 10ms typical
Range: Up to 200m line-of-sight (environment dependent)
Data rate: Up to 250 packets/second
Payload: Up to 250 bytes per packet

## Troubleshooting

### No data received

Verify both devices are on the same WiFi channel
Check MAC address is correctly configured in transmitter
Ensure both devices use matching data structures

### Packet loss

Reduce transmission rate
Increase queue size on receiver
Check for WiFi interference

### Build errors

Verify ESP-IDF version is v5.4.1 or later
Ensure all components are properly linked in CMakeLists.txt

## Contributing

Contributions welcome! Please submit pull requests or open issues for bugs and feature requests.