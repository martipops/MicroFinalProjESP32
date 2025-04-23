#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

// Function declarations
bool setupWiFi();
void setupAP();
bool isWiFiConnected();
IPAddress getLocalIP();

#endif