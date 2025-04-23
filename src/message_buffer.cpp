#include "message_buffer.h"

// Message buffer variables
static String messageBuffer[MESSAGE_BUFFER_SIZE];
static int messageCount = 0;
static SemaphoreHandle_t bufferMutex;

void initMessageBuffer() {
  bufferMutex = xSemaphoreCreateMutex();
}

bool addMessageToBuffer(String message) {
  bool success = false;
  if (xSemaphoreTake(bufferMutex, (TickType_t)10) == pdTRUE) {
    // If buffer is full, shift all messages to make room
    if (messageCount >= MESSAGE_BUFFER_SIZE) {
      // Shift all messages one position
      for (int i = 0; i < MESSAGE_BUFFER_SIZE - 1; i++) {
        messageBuffer[i] = messageBuffer[i + 1];
      }
      messageCount = MESSAGE_BUFFER_SIZE - 1;
    }
    
    // Add new message at the end
    messageBuffer[messageCount++] = message;
    Serial.println("Added message to buffer: " + message);
    
    xSemaphoreGive(bufferMutex);
    success = true;
  }
  return success;
}

String* getMessages(int* count) {
  if (xSemaphoreTake(bufferMutex, (TickType_t)10) == pdTRUE) {
    *count = messageCount;
    xSemaphoreGive(bufferMutex);
    return messageBuffer;
  }
  *count = 0;
  return NULL;
}

bool clearMessages() {
  bool success = false;
  if (xSemaphoreTake(bufferMutex, (TickType_t)10) == pdTRUE) {
    messageCount = 0;
    xSemaphoreGive(bufferMutex);
    success = true;
  }
  return success;
}