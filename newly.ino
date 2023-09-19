#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ESP8266WiFi.h>

const char* ssid = "ZTE_2.4G_3tCLTE";
const char* password = "V5jd5pnf";

const int trigPin = D1;
const int echoPin = D2;
#define USONIC_DIV 0.034

long duration;
long distance;
WebSocketsServer webSocket(81); // WebSocket server on port 81

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
  measureDistance();
}

void measureDistance() {
  int percentage;
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(11);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * USONIC_DIV / 2;
  percentage = map(distance, 20, 4, 0, 100);
  

  if (percentage < 0) {
    percentage = 0;
  } else if (percentage > 100) {
    percentage = 100;
  }

  // Prints the distance to Serial Monitor (for debugging)
  Serial.print("Percentage: ");
  Serial.print(percentage);
  Serial.print("% Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  delay(1000);
  // Send data to connected clients (e.g., Android app) via WebSocket
  String dataToSend = String(percentage) + "," + String(distance);
  webSocket.broadcastTXT(dataToSend);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      Serial.printf("[%u] Client connected\n", num);
      break;
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Client disconnected\n", num);
      break;
    case WStype_TEXT:
      Serial.printf("[%u] Received text: %s\n", num, payload);
      break;
  }
}
