#include <Arduino.h>
#include <WiFi.h>
#include "ota.h"
#include "secrets.h"

const char* appversion = "7.7.7";

unsigned long ticks = 0;

void printVersion(void) {
  if (millis() - ticks > 5000) {
    Serial.printf("Version %s\n", appversion);
    ticks = millis();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  ticks = millis() - 20000;
  printVersion();
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifissid, wifipwd);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  setupOTA(hostname, otaport, otapwd);

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  handleOTA();
  printVersion();
}
