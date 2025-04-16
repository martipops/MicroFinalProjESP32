#include <Arduino.h>
#include <WiFi.h>           // ESP32 WiFi library
#include <WebServer.h>      // ESP32 WebServer library
#include <ArduinoJson.h>    // JSON support
#include "index_html.h"     // Generated header file
#include "secrets.h"        // WiFi credentials

// Station credentials
const char* sta_ssid = WIFI_SSID;
const char* sta_password = WIFI_PASSWORD;

// Access Point credentials (fallback)
const char* ap_ssid = AP_SSID;
const char* ap_password = AP_PASSWORD;

const int READING_COUNT = 20;

// ESP32 pin definitions
const int GAS_SENSOR_PIN = 34;    // ADC1_CH0 (GPIO 36)
const int SOUND_SENSOR_PIN = 35;  // ADC1_CH3 (GPIO 39)
const int MOTION_SENSOR_PIN = 32; // Digital pin for PIR

// Arrays to store sensor readings
static int gasLevels[READING_COUNT];
static int soundLevels[READING_COUNT];
static bool motionLevels[READING_COUNT];

// WiFi connection timeout
const unsigned long WIFI_TIMEOUT = 10000; // 30 seconds

// Timing for sensor readings without blocking
unsigned long lastReadingTime = 0;
unsigned long dataInterval = 100;

// Web server on port 80 for HTTP
WebServer server(80);

// Handler for the /read endpoint
void handleReadRoute() {
  // Create a fresh JSON document for each response to avoid memory issues
  StaticJsonDocument<1024> doc;
  
  // Add sensor data arrays to the JSON document
  JsonArray gasArray = doc.createNestedArray("gas");
  JsonArray soundArray = doc.createNestedArray("sound");
  JsonArray motionArray = doc.createNestedArray("motion");
  
  // Fill arrays with sensor data
  for (int i = 0; i < READING_COUNT; i++) {
    gasArray.add(gasLevels[i]);
    soundArray.add(soundLevels[i]);
    motionArray.add(motionLevels[i]);
  }
  
  // Serialize JSON to string
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
    // Non-blocking delay - allow other operations during WiFi connection
    unsigned long connectStart = millis();
    while (millis() - connectStart < 500) {
      // Handle any other tasks here if needed during connection
      yield(); // Allow system to handle other tasks
    }
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

void initializeLevels() {
  // Initialize the sensor readings arrays
  for (int i = 0; i < READING_COUNT; i++) {
    gasLevels[i] = 0;
    soundLevels[i] = 0;
    motionLevels[i] = false;
  }
}

void takeReadings() {
  // Shift all readings one position to make room for new reading
  for (int i = 0; i < READING_COUNT - 1; i++) {
    gasLevels[i] = gasLevels[i + 1];
    soundLevels[i] = soundLevels[i + 1];
    motionLevels[i] = motionLevels[i + 1];
  }
  
  // Add new readings at the end of arrays
  gasLevels[READING_COUNT - 1] = analogRead(GAS_SENSOR_PIN);
  soundLevels[READING_COUNT - 1] = analogRead(SOUND_SENSOR_PIN);
  motionLevels[READING_COUNT - 1] = digitalRead(MOTION_SENSOR_PIN);
  
  // Debug output
  Serial.printf("New readings - Gas: %d, Sound: %d, Motion: %d\n", 
                gasLevels[READING_COUNT - 1], 
                soundLevels[READING_COUNT - 1], 
                motionLevels[READING_COUNT - 1]);
}

void setup() {
  // Start serial communication
  Serial.begin(115200);
  Serial.println();

  // Set up pins
  pinMode(MOTION_SENSOR_PIN, INPUT);

  // ESP32 ADC configuration
  analogReadResolution(12); // ESP32 has 12-bit ADC (0-4095)
  
  initializeLevels();

  // Try to connect to WiFi first, fallback to AP mode if needed
  if (!connectToWiFi()) {
    setupAP();
  }

  server.on("/api/interval", HTTP_POST, []() {
    if (server.hasArg("value")) {
      String value = server.arg("value");
      dataInterval = value.toInt();
      
      // Return success response
      server.send(200, "application/json", "{\"success\":true,\"interval\":" + String(dataInterval) + "}");
    } else {
      // Return error if no value parameter is provided
      server.send(400, "application/json", "{\"success\":false,\"message\":\"Missing value parameter\"}");
    }
  });

  // Initialize the server routes
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Content-Encoding", "gzip");
    server.sendHeader("Cache-Control", "max-age=86400");
    server.send_P(200, "text/html", (const char*) index_html_gz, index_html_gz_len);
  });
  
  server.on("/read", HTTP_GET, handleReadRoute);
  
  // Start the server
  server.begin();
  Serial.println("HTTP server started");
  
  // Initialize lastReadingTime
  lastReadingTime = millis();
}

void loop() {
  // Handle client requests with highest priority
  server.handleClient();
  
  // Non-blocking sensor reading based on timing
  unsigned long currentMillis = millis();
  if (currentMillis - lastReadingTime >= dataInterval) {
    takeReadings();
    lastReadingTime = currentMillis;
  }
  
  // Small yield to allow other processes to run
  yield();
}