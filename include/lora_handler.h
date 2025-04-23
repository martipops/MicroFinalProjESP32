#ifndef LORA_HANDLER_H
#define LORA_HANDLER_H

#include <Arduino.h>

// Define LoRa UART pins for ESP32
#define LORA_RX 16  // ESP32 pin for receiving data from LoRa
#define LORA_TX 17  // ESP32 pin for sending data to LoRa

// Function declarations
void initLoRa();
void loraTask(void *parameter);

extern TaskHandle_t loraTaskHandle;

#endif