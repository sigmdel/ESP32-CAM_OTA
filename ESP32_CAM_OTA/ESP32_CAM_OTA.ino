
/*
 * ESP32_CAM_OTA
 *
 * (c) Copyright, 2022, Michel Deslierres
 * The  Zero Clause BSD license applies (see https://opensource.org/licenses/0BSD)
 * SPDX-License-Identifier: 0BSD
 *
 * ESP32_CAM_OTA is a reworked version of the BasicOTA example for the ESP32-CAM.
 * The BasicOTA example for ArduinoOTA library will would not run with an ESP32-CAM (clone?)
 * using the "AI Thinker ESP32-CAM" card. The Arduino or PlatformIO IDE does send an
 * invitation to the ESP32-CAM and OTA starts but it is immediately aborted with the
 * ESP32-CAM sending the following message on its serial port
 *
 * : Start updating sketch
 * : Progress: 0%
 * : abort() was called at PC 0x40081dd1 on core 1
 *
 * Then ESP32 reboots. This is a known problem:
 *
 *    BasicOTA and OTAWebUpdater fails with "abort() was called at PC 0x40136e85 on core 1"
 *      @  https: *www.esp32.com/viewtopic.php?t=17133
 *
 *    OTA does not work on AI Thinker ESP32-CAM board
 *      @  https: *esp32.com/viewtopic.php?t=14700
 *
 * The problems is the memory partition in the ESP32-CAM board definition. The proposed
 * solution fall into two camps:
 *
 *   1. Modify the ESP32-CAM board definition, which will have to be repeated each time
 *      the esp32 Arduino core is updated.
 *
 *   2. a) In the Arduino IDE, use a different board definition such as
 *
 *      | Card: "ESP32 Wrover Kit (all versions)"
 *      | Upload Speed: "921600"
 *      | CPU Frequency: "240 MHz (WiFi/BT)"
 *      | Flash Frequency: "80MHz"
 *      | Flash Mode: "QIO"
 *      | Flash Size: "4MB (32Mb)"
 *      | Partition Scheme: "Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS")
 *      | Core Debug Level: "None"
 *      | PSRAM: "Enabled"
 *
 *      All the entries except for the Partition Scheme are default values.
 *      Three partitions schemes seem to work
 *        "Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS"
 *        "Default 4MB with ffat (1.2MB APP/1.5MB FATFS"
 *        "Minimal SPIFFS (1.9MB APP/190KB SPIFFS"
 *
 *   2. b) In PlatformIO, add a memory partition option in the esp32cam environement section
 *         of platformio.ini
 *
 *      | [env:esp32cam]
 *      | platform = espressif32
 *      | board = esp32cam
 *      | board_build.partitions = min_spiffs.csv  ; needed for OTA
 *      | framework = arduino
 *      | monitor_speed = 115200
 *
 *
 * The ota.h/ota.cpp files isolates most of the OTA code from original BasicOTA source to make
 * it easier to add "push" OTA in other projects. These files are a simplification of
 *
 *   ESP32 OTA @ https://github.com/SensorsIot/ESP32-OTA
 *   by SensorsIot (Andreas Spiess - The guy with the Swiss accent)
 *
 * If using OpenRTOS then please consult the original post.
 *
 * This .INO file is nothing but a long comment but it nevertheless satisfies the Arduino requirements.
 * The true source of the project is in the file main.cpp.
 *
 * DO NOT FORGET to edit the settings in secrets-template.h file and to save it as secrets.h.
 * In PlatformIO edit the upload_port and upload_flags settings to match the values in secrets.h.
 */
