#ifndef MESSAGE_BUFFER_H
#define MESSAGE_BUFFER_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// Message buffer configuration
const int MESSAGE_BUFFER_SIZE = 200;

// Public API
void initMessageBuffer();
bool addMessageToBuffer(String message);
String* getMessages(int* count);
bool clearMessages();

#endif