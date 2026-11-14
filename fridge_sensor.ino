#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

const char* ssid           = "ryusei";
const char* password       = "ryuseisimp";
const char* discordWebhook = "https://discord.com/api/webhooks/1563346772946980864/eBm5bQQq3O2XR05TnJjesmNBrU3oOuKNrBiPCCZdUVTT8__H_GG_3o5R2eatsDK6o15r";

const int reedPin = 5;

const unsigned long SEND_INTERVAL_MS = 5000;
unsigned long lastSendTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // External pull-down on the circuit, so no internal resistor
  pinMode(reedPin, INPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  lastSendTime = millis() - SEND_INTERVAL_MS;  // send first message right away
}

void loop() {
  if (millis() - lastSendTime >= SEND_INTERVAL_MS) {
    lastSendTime = millis();

    bool magnetPresent = (digitalRead(reedPin) == HIGH);
    String msg = magnetPresent ? "Magnet detected (door CLOSED)"
                               : "No magnet (door OPEN)";

    Serial.println(msg);
    sendDiscordMessage(msg);
  }
}

void sendDiscordMessage(String message) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, reconnecting...");
    WiFi.reconnect();
    return;
  }

  WiFiClientSecure client;
  client.setInsecure();
  client.setTimeout(10000);

  HTTPClient http;
  if (!http.begin(client, discordWebhook)) {
    Serial.println("http.begin() failed");
    return;
  }

  http.addHeader("Content-Type", "application/json");
  int code = http.POST("{\"content\":\"" + message + "\"}");

  Serial.print("Discord response: ");
  Serial.println(code);
  if (code < 0) Serial.println(HTTPClient::errorToString(code));

  http.end();
}