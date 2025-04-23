#include "wifi_manager.h"
#include "secrets.h"

// WiFi credentials
const char* sta_ssid = WIFI_SSID;
const char* sta_password = WIFI_PASSWORD;

// Access Point credentials (fallback)
const char* ap_ssid = AP_SSID;
const char* ap_password = AP_PASSWORD;

// WiFi connection timeout
const unsigned long WIFI_TIMEOUT = 10000; // 10 seconds

bool setupWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(sta_ssid, sta_password);
  
  // Try to connect with timeout
  unsigned long startAttemptTime = millis();
  
  while (WiFi.status() != WL_CONNECTED && 
         millis() - startAttemptTime < WIFI_TIMEOUT) {
    Serial.print(".");
    delay(500);
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("Connected to WiFi! IP address: ");
    Serial.println(WiFi.localIP());
    return true;
  } else {
    Serial.println();
    Serial.println("Failed to connect to WiFi");
    WiFi.disconnect();
    return false;
  }
}

void setupAP() {
  // Start the Access Point
  Serial.print("Starting Access Point...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);
  Serial.println(" Done!");
  
  // Print the IP address of the ESP32
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());
}

bool isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

IPAddress getLocalIP() {
  if (WiFi.getMode() == WIFI_AP) {
    return WiFi.softAPIP();
  } else {
    return WiFi.localIP();
  }
}