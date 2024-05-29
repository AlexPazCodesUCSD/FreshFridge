#include <WiFi.h>
#include <Wire.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "webpage.h"

// Replace with your network credentials
const char* ssid = "MAiPhone";
const char* password = "singapore";

// http://172.20.10.11/

#define SDA_PIN 23
#define SCL_PIN 22

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

int sensorValue = 0;
unsigned long previousMillis = 0;  // Stores the last time sensor data was updated
long interval = 1000;              // Interval at which to read the sensor (in milliseconds)

void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
                      AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.println("WebSocket client connected");
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("WebSocket client disconnected");
  } else if (type == WS_EVT_DATA) {
    String message = String((char*)data).substring(0, len);
    Serial.print("Received slider value: ");
    Serial.println(message);
    interval = message.toInt();  // Update the interval based on the slider value
  }
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // WebSocket server
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);

  // Serve HTML content
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", htmlContent);
  });

  // Start server
  server.begin();
}

void loop() {
  unsigned long currentMillis = millis();

  // Check if the interval has passed to read sensor data
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Read sensor data
    Wire.beginTransmission(0x30);
    Wire.endTransmission();
    Wire.requestFrom(0x30, 2);
    if (Wire.available() == 2) {
      byte msb = Wire.read();
      byte lsb = Wire.read();

      sensorValue = ((msb << 8) | lsb);
      Serial.print("Sensor Value (");
      Serial.print(interval);
      Serial.print(" ms): ");
      Serial.println(sensorValue);
    }

    // Send sensor data to all WebSocket clients
    String message = String(sensorValue);
    ws.textAll(message);
  }

  ws.cleanupClients();
}
