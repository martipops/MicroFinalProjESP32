#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>  // Add this include for JSON support
#include "index_html.h" // Include the generated header file
#include "secrets.h" // Include the secrets header for WiFi credentials

// Station credentials (connect to this network)
const char* sta_ssid = WIFI_SSID;  // Change to your WiFi network name
const char* sta_password = WIFI_PASSWORD; // Change to your WiFi password

// Access Point credentials (fallback if can't connect to WiFi)
const char* ap_ssid = AP_SSID;
const char* ap_password = AP_PASSWORD;

// WiFi connection timeout
const unsigned long WIFI_TIMEOUT = 30000; // 30 seconds

// Create an instance of the ESP8266WebServer
ESP8266WebServer server(80); // Port 80 for HTTP

// New handler for the /read endpoint
void handleRead() {
  // Read the analog and digital values
  int analogValue = analogRead(A0);  // ESP8266 has only one analog pin (A0)
  int digitalValue = digitalRead(14); // D5 on NodeMCU is GPIO14
  
  // Create JSON response
  StaticJsonDocument<200> doc;
  doc["analog"] = analogValue;
  doc["digital"] = digitalValue;
  
  String jsonResponse;
  serializeJson(doc, jsonResponse);
  
  // Send the JSON response
  server.send(200, "application/json", jsonResponse);
}

bool connectToWiFi() {
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
  
  // Print the IP address of the ESP8266
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());
}

void setup() {
  // Start serial communication
  Serial.begin(115200);
  Serial.println();

  // Set up pins
  pinMode(D7, INPUT);  // D7 as digital input

  // Try to connect to WiFi first, fallback to AP mode if needed
  if (!connectToWiFi()) {
    setupAP();
  }

  // Initialize the server
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Content-Encoding", "gzip");
    server.sendHeader("Cache-Control", "max-age=86400");
    server.send_P(200, "text/html", (const char*) index_html_gz, index_html_gz_len);
  });
  
  server.on("/read", handleRead);  // Add the new endpoint
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Handle incoming client requests
  server.handleClient();
}