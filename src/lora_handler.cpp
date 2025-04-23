#include "lora_handler.h"
#include "message_buffer.h"

// Hardware Serial for LoRa communication
HardwareSerial loraSerial(1); // Use UART1 for LoRa
TaskHandle_t loraTaskHandle = NULL;

void initLoRa() {
  // Initialize LoRa serial communication
  loraSerial.begin(115200, SERIAL_8N1, LORA_RX, LORA_TX);
  Serial.println("LoRa serial initialized");
}

void loraTask(void *parameter) {
  String inString = "";
  
  while (true) {
    // Read from LoRa module
    while (loraSerial.available()) {
      char inChar = (char)loraSerial.read();
      
      // Process when newline is received
      if (inChar == '\n') {
        if (inString.length() > 0) {
          // Add the received message to buffer
          addMessageToBuffer(inString);
          inString = "";
        }
      } else {
        inString += inChar;
      }
    }
    
    // Small delay to prevent this task from hogging all CPU time
    vTaskDelay(1); // Minimal delay to allow other tasks to run
  }
}