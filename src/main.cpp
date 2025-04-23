#include <Arduino.h>
#include <SoftwareSerial.h>

// LoRa module connection pins
const int LORA_RX = 2;  // Connect to TX on LoRa module
const int LORA_TX = 3;  // Connect to RX on LoRa module
const int LED_PIN = 13; // Built-in LED to indicate message received

// Create LoRa serial communication
SoftwareSerial loraSerial(LORA_RX, LORA_TX);  // RX, TX

// Variables for message handling
String currentMessage = "";
bool messageReceived = false;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  // Initialize LoRa module
  loraSerial.begin(115200);
  Serial.println("LoRa Receiver Initialized");
  
  // Set up LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Send a startup message
  Serial.println("Waiting for LoRa messages...");
}

void loop() {
  // Check for incoming data from LoRa module
  while (loraSerial.available()) {
    char inChar = (char)loraSerial.read();
    
    if (inChar == '\n') {
      // End of message detected
      messageReceived = true;
    } else {
      // Add character to current message
      currentMessage += inChar;
    }
  }

  // Process complete message if one was received
  if (messageReceived) {
    // Flash LED to indicate message received
    digitalWrite(LED_PIN, HIGH);
    
    // Display the received message
    Serial.print("Received: ");
    Serial.println(currentMessage);
    
    // Parse the message if needed
    // Example format: "SENSOR:DIST:123"
    if (currentMessage.startsWith("SENSOR:")) {
      int distIndex = currentMessage.indexOf("DIST:");
      if (distIndex >= 0) {
        String distValue = currentMessage.substring(distIndex + 5);
        int distance = distValue.toInt();
        
        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.println(" cm");
      }
    }
    
    // Reset for next message
    currentMessage = "";
    messageReceived = false;
    
    // Turn off LED after brief flash
    delay(100);
    digitalWrite(LED_PIN, LOW);
  }
}