#include <WiFi.h>
#include <HTTPClient.h>

// Replace with your WiFi credentials
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Replace with your API Gateway URL
const String lambda_url = "https://<api-id>.execute-api.<region>.amazonaws.com/prod/sendSms";

const int trigPin = 5;    // GPIO pin for TRIG
const int echoPin = 18;   // GPIO pin for ECHO

long duration;
float distance;
const float safeLevel = 30.0;  // Threshold: Water within 30 cm indicates potential flood

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // Measure water level (distance from sensor to water surface)
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.0343) / 2;  // Calculate distance in cm

  // Calculate the inverse for water level: a lower distance means higher water level
  if (distance <= safeLevel) {  // If water level is above safe threshold
    Serial.println("Potential flood detected!");
    Serial.println("Sending alert...");

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(lambda_url);
      http.addHeader("Content-Type", "application/json");

      // Payload with water level data
      String payload = "{\"distance\": " + String(distance) + "}";
      int httpResponseCode = http.POST(payload);

      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
      } else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
      }

      http.end();
    }
  } else {
    Serial.print("Water level is safe: ");
    Serial.print(distance);
    Serial.println(" cm");
  }

  // Wait 2 seconds before the next measurement
  delay(2000);
}
