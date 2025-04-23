#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <WebServer.h>

// Function declarations
void initWebServer();
void serverTask(void *parameter);

extern WebServer server;
extern TaskHandle_t serverTaskHandle;

#endif