#include <WiFi.h>
#include <HTTPClient.h>

// ---------- WiFi ----------
const char* ssid = "ryusei";
const char* password = "ryuseisimp";

// ---------- Discord ----------
const char* discordWebhook =
  "https://discord.com/api/webhooks/1563346772946980864/eBm5bQQq3O2XR05TnJjesmNBrU3oOuKNrBiPCCZdUVTT8__H_GG_3o5R2eatsDK6o15r";

// ---------- Input ----------
const int inputPin = 5;   // GPIO5 -- verify this for your ESP32 board

// ---------- Timing ----------
const unsigned long LOW_TIME_REQUIRED = 5000;  // 5 seconds

unsigned long lowStartTime = 0;
bool timingLow = false;
bool messageSent = false;


void setup() {
  Serial.begin(115200);

  // D5 is being driven externally by 0 V / 3.3 V
  pinMode(inputPin, INPUT);

  // Connect to WiFi
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void loop() {

  int pinState = digitalRead(inputPin);

  // -------------------------
  // D5 is LOW
  // -------------------------
  if (pinState == LOW) {

    // Just became LOW -- start timer
    if (!timingLow) {
      timingLow = true;
      lowStartTime = millis();

      Serial.println("D5 went LOW. Starting 5-second timer...");
    }

    // Has it stayed LOW for 5 seconds?
    if (!messageSent &&
        (millis() - lowStartTime >= LOW_TIME_REQUIRED)) {

      Serial.println("D5 has been LOW for 5 seconds.");

      sendDiscordMessage("hello world");

      messageSent = true;
    }
  }

  // -------------------------
  // D5 is HIGH
  // -------------------------
  else {

    // Reset everything
    if (timingLow) {
      Serial.println("D5 went HIGH. Timer reset.");
    }

    timingLow = false;
    messageSent = false;
  }

  delay(10);
}


void sendDiscordMessage(String message) {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi is not connected!");
    return;
  }

  HTTPClient http;

  http.begin(discordWebhook);
  http.addHeader("Content-Type", "application/json");

  String jsonPayload = "{\"content\":\"" + message + "\"}";

  int httpResponseCode = http.POST(jsonPayload);

  if (httpResponseCode > 0) {
    Serial.print("Discord response: ");
    Serial.println(httpResponseCode);
  }
  else {
    Serial.print("Discord POST failed: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}