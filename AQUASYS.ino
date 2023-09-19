#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>

const int trigPin = D1;
const int echoPin = D2;
#define USONIC_DIV 0.034

long duration;
long distance;

ESP8266WebServer server(80);

void setup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin("ZTE_2.4G_3tCLTE", "V5jd5pnf");

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);  // Sets the echoPin as an Input

  digitalWrite(trigPin, LOW);
  delayMicroseconds(1000);

  Serial.begin(9600);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println("ZTE_2.4G_3tCLTE");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/get-distance", HTTP_GET, handleUltrasonicSensor);

  server.begin();
}

void loop() {
  server.handleClient();
}

void handleUltrasonicSensor() {
  int percentage;
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(11);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * USONIC_DIV / 2;
  percentage = map(distance, 14, 4, 0, 100);

  if (percentage < 0) {
    percentage = 0;
  } else if (percentage > 100) {
    percentage = 100;
  }

  // Print the request to the Serial Monitor for debugging
  Serial.println("Received /get-distance request");

  // Prints the distance on the Serial Monitor
  Serial.print("Percentage: ");
  Serial.print(percentage);
  Serial.print("% Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  delay(2000);

  String response = String(percentage) + "," + String(distance);

  server.send(200, "application/json", response);
}
