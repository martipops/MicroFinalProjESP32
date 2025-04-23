#include "web_server.h"
#include "message_buffer.h"
#include "index_html.h"
#include <ArduinoJson.h>

// Web server on port 80 for HTTP
WebServer server(80);
TaskHandle_t serverTaskHandle = NULL;

// Handler for the /read endpoint
void handleReadRoute() {
  // Create a fresh JSON document
  StaticJsonDocument<16384> doc;
  JsonArray messagesArray = doc.createNestedArray("messages");
  
  // Get messages from the buffer
  int count;
  String* messages = getMessages(&count);
  
  if (messages != NULL) {
    // Add all messages to the JSON array
    for (int i = 0; i < count; i++) {
      messagesArray.add(messages[i]);
    }
    
    // Serialize JSON to string
    String jsonResponse;
    serializeJson(doc, jsonResponse);
    
    // Send the JSON response
    server.send(200, "application/json", jsonResponse);
  } else {
    // If we couldn't get the messages, return an error
    server.send(503, "application/json", "{\"error\":\"Server busy\"}");
  }
}

// Handler to clear the message buffer
void handleClearRoute() {
  if (clearMessages()) {
    server.send(200, "application/json", "{\"success\":true,\"message\":\"Buffer cleared\"}");
  } else {
    server.send(503, "application/json", "{\"error\":\"Server busy\"}");
  }
}
void handleLoadRoute() {
  // Load the HTML page
  server.sendHeader("Content-Encoding", "gzip");
  server.sendHeader("Cache-Control", "max-age=86400");
  server.send_P(200, "text/html", (const char*) index_html_gz, index_html_gz_len);
}

void initWebServer() {
  // Initialize the server routes
  server.on("/", HTTP_GET, handleLoadRoute);
  server.on("/read", HTTP_GET, handleReadRoute);
  server.on("/clear", HTTP_GET, handleClearRoute);
  
  // Start the server
  server.begin();
  Serial.println("HTTP server started");
}

void serverTask(void *parameter) {
  while (true) {
    server.handleClient();
    vTaskDelay(1); // Minimal delay to allow other tasks to run
  }
}