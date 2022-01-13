#include <ArduinoOTA.h>

void setupOTA(const char* hostname="", uint16_t port=3232, const char* pwd="", const char* pswdhash="") {
  // Port defaults to 3232
  // Hostname defaults to esp3232-[MAC]
  // No authentication by default
  // Password can be set with it's md5 value as well
  if (strlen(hostname))
    ArduinoOTA.setHostname(hostname);
  if (port && port!=3232)
    ArduinoOTA.setPort(port);
  if (strlen(pswdhash))
    ArduinoOTA.setPasswordHash(pswdhash);
  else if (strlen(pwd))
    ArduinoOTA.setPassword(pwd);

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
}

void handleOTA(void) {
  ArduinoOTA.handle();
}
