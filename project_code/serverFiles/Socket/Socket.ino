#include <SPIFFS.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "SparkFun_SCD4x_Arduino_Library.h"

const char* ssid = "SonalSax";
const char* password = "ArnieSexPics";

#define SDA_PIN 23
#define SCL_PIN 22

const int methanePin = A0;
int methaneValue = 0;

SCD4x CO2Sensor;

int sensorValue = 0;
unsigned long previousMillis = 0;  // Stores the last time sensor data was updated
long interval = 1000;              // Interval at which to read the sensor (in milliseconds)

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

 // Print the IP address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Wire.begin(SDA_PIN, SCL_PIN);

  if (CO2Sensor.begin() == false)
  {
    Serial.println(F("Sensor not detected. Please check wiring. Freezing..."));
    while (1)
      ;
  }

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
    return;
  }
  Serial.println("SPIFFS mounted successfully");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // Route for container1_html.html
  server.on("/container1_html.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/container1_html.html", "text/html");
  });

  // Route for container2_html.html
  server.on("/container2_html.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/container2_html.html", "text/html");
  });

  // Route for container3_html.html
  server.on("/container3_html.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/container3_html.html", "text/html");
  });

  // Route for container4_html.html
  server.on("/container4_html.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/container4_html.html", "text/html");
  });

  // WebSocket event handler
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // Start server
  server.begin();
}

void loop() {
  unsigned long currentMillis = millis();

  // Check if the interval has passed to read sensor data
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    if (CO2Sensor.readMeasurement()) { // readMeasurement will return true when fresh data is available
      float co2 = CO2Sensor.getCO2();
      Serial.print(F("CO2 (ppm): "));
      Serial.println(co2);

      // Send CO2 data to all connected WebSocket clients
      String jsonMessage = "{\"sensorValue1\": " + String(co2) + "}";
      ws.textAll(jsonMessage);
    }
  }
  
  ws.cleanupClients();
}

// WebSocket event handler function
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("WebSocket client connected: %u\n", client->id());
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("WebSocket client disconnected: %u\n", client->id());
  } else if (type == WS_EVT_DATA) {
    data[len] = 0;
    Serial.printf("WebSocket message received: %s\n", (char*)data);
    client->text("Message received: " + String((char*)data));
  }
}
