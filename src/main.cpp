#include <Arduino.h>
#include <WiFi.h>           // ESP32 WiFi library
#include <WebServer.h>      // ESP32 WebServer library
#include <ArduinoJson.h>    // JSON support
#include <HardwareSerial.h> // For LoRa communication
#include "index_html.h"     // Generated header file
#include "secrets.h"        // WiFi credentials

// Station credentials
const char* sta_ssid = WIFI_SSID;
const char* sta_password = WIFI_PASSWORD;

// Access Point credentials (fallback)
const char* ap_ssid = AP_SSID;
const char* ap_password = AP_PASSWORD;

// WiFi connection timeout
const unsigned long WIFI_TIMEOUT = 10000; // 10 seconds

// Web server on port 80 for HTTP
WebServer server(80);

// LoRa communication
HardwareSerial LoRaSerial(2); // Use UART2 (TX=17, RX=16)
const int MAX_LORA_MESSAGES = 20;
String loraMessages[MAX_LORA_MESSAGES]; // Circular buffer for LoRa messages
int messageIndex = 0;

// Handler for the /read endpoint
void handleReadRoute() {
  // Create a fresh JSON document for each response
  StaticJsonDocument<2048> doc;
  
  // Add LoRa messages to the JSON document
  JsonArray messagesArray = doc.createNestedArray("messages");
  
  // Fill array with LoRa messages
  for (int i = 0; i < MAX_LORA_MESSAGES; i++) {
    if (!loraMessages[i].isEmpty()) {
      messagesArray.add(loraMessages[i]);
    }
  }
  
  // Serialize JSON to string
  String jsonResponse;
  serializeJson(doc, jsonResponse);
  
  // Send the JSON response
  server.send(200, "application/json", jsonResponse);
}

void handleClearRoute() {
  // Clear all messages in the buffer
  for (int i = 0; i < MAX_LORA_MESSAGES; i++) {
    loraMessages[i] = "";
  }
  
  // Reset message index
  messageIndex = 0;
  
  // Create a response JSON
  StaticJsonDocument<128> doc;
  doc["status"] = "success";
  doc["message"] = "LoRa message buffer cleared";
  
  // Serialize JSON to string
  String jsonResponse;
  serializeJson(doc, jsonResponse);
  
  // Send the response
  server.send(200, "application/json", jsonResponse);
  
  Serial.println("LoRa message buffer cleared via API");
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

void addLoRaMessage(String message) {
  for (int i = MAX_LORA_MESSAGES - 1; i > 0; i--) {
    loraMessages[i] = loraMessages[i-1];
  }
  loraMessages[0] = message;
  
  Serial.println("Received LoRa: " + message);
}

void configureLoRa() {
  // Configure LoRa Module for Group 4
  LoRaSerial.println("AT+BAND=915800000"); // Set frequency to 915.8 MHz
  delay(100);
  
  LoRaSerial.println("AT+ADDRESS=8"); // Receiver address for Group 4
  delay(100);
  
  LoRaSerial.println("AT+NETWORKID=24"); // Set Network ID for Group 4
  delay(100);
  
  Serial.println("LoRa Receiver Ready for Group 4.");
}

void checkLoRaMessages() {
  if (LoRaSerial.available()) {
    String receivedData = LoRaSerial.readString();
    
    // Add message to buffer
    addLoRaMessage(receivedData);
    
    // Only respond if message belongs to Group 4
    if (receivedData.indexOf("distan") >= 0) {
      Serial.println("Replying with G4_PONG...");
      LoRaSerial.println("AT+SEND=7,6,G4_PON"); // Reply to sender
    }
  }
}

void setup() {
  // Start serial communication
  Serial.begin(115200);
  Serial.println();

  // Initialize LoRa communication
  LoRaSerial.begin(115200, SERIAL_8N1, 16, 17);
  delay(2000); // Give time for LoRa module to initialize
  
  // Configure LoRa module
  configureLoRa();
  
  // Initialize message buffer
  for (int i = 0; i < MAX_LORA_MESSAGES; i++) {
    loraMessages[i] = "";
  }

  // Try to connect to WiFi first, fallback to AP mode if needed
  if (!connectToWiFi()) {
    setupAP();
  }

  // Initialize the server routes
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Content-Encoding", "gzip");
    server.sendHeader("Cache-Control", "max-age=86400");
    server.send_P(200, "text/html", (const char*) index_html_gz, index_html_gz_len);
  });
  
  server.on("/read", HTTP_GET, handleReadRoute);

  server.on("/clear", HTTP_GET, handleClearRoute);
  
  // Start the server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Handle client requests with highest priority
  server.handleClient();
  
  // Check for new LoRa messages
  checkLoRaMessages();
  
  // Small yield to allow other processes to run
  yield();
}