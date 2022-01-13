# ESP32-CAM OTA with ArduinoOTA

**ESP32_CAM_OTA** is a reworked version of the **BasicOTA** example for the ESP32-CAM.

# Table of Content
<!-- TOC -->

- [1. Problem](#1-problem)
- [2. Cause](#2-cause)
- [3. Solutions in the Arduino Environment](#3-solutions-in-the-arduino-environment)
- [4. Solutions in the PlatformIO Environement](#4-solutions-in-the-platformio-environement)
- [5. Simplifying the use of ArduionoOTA](#5-simplifying-the-use-of-arduionoota)
- [6. Last Comments and Instructions](#6-last-comments-and-instructions)
- [7. License](#7-license)

<!-- /TOC -->
## 1. Problem

There is a well-known problem with using the [ArduinoOTA](https://github.com/espressif/arduino-esp32/tree/master/libraries/ArduinoOTA) library with the AI Thinker ESP32-CAM board and its many clones. See for example [BasicOTA and OTAWebUpdater fails with "abort() was called at PC 0x40136e85 on core 1"](https:www.esp32.com/viewtopic.php?t=17133) and [OTA does not work on AI Thinker ESP32-CAM board](https:esp32.com/viewtopic.php?t=14700).

When attempting to update the card's firmware in the `BasicOTA` example included in the library, the board does receive the invitation to start an upload. However, the operation is immediately interrupted and the ESP32-CAM sends the following message on its serial port.

```
Start updating sketch
Progress: 0%
abort() was called at PC 0x40081dd1 on core 1
```

The ESP32 then reboots.

## 2. Cause

The problem has nothing to do with the **ArduinoOTA** library. Its source is the memory partition in the ESP32-CAM board definition. The `esp32cam.build.partition=huge_app` setting in the board definition file ([board.txt](https://github.com/espressif/arduino-esp32/blob/master/boards.txt)), specify that most of the memory is allocated to a single app partition (3MiB in size) leaving no room in which to buffer an uploaded firmware update. 

<!--

```
sp32cam.upload.maximum_size=3145728
esp32cam.upload.maximum_data_size=327680
...
esp32cam.build.flash_size=4MB
esp32cam.build.partitions=huge_app
esp32cam.build.defines=-DBOARD_HAS_PSRAM -mfix-esp32-psram-cache-issue -mfix-esp32-psram-cache-strategy=memw
```
it becomes clear that there is no space left for storing the 

[huge_app.csv](https://github.com/espressif/arduino-esp32/blob/master/tools/partitions/huge_app.csv):

# Name |	Type 	| SubType |	Offset |	Size 
 ---   |  ---   |  ---    |  ---   |  ---   
nvs |	data |	nvs |	0x9000 |	0x5000 	
otadata |	data |	ota |	0xe000 |	0x2000 	
app0 |	app |	ota_0 |	0x10000 |	0x300000 	
spiffs |	data |	spiffs |	0x310000 |	0xF0000 


[default.csv](https://github.com/espressif/arduino-esp32/blob/master/tools/partitions/default.csv)

# Name |	Type 	| SubType |	Offset |	Size 
 ---   |  ---   |  ---    |  ---   |  ---   
nvs |	data |	nvs |	0x9000 |	0x5000 	
otadata |	data |	ota |	0xe000 |	0x2000 	
app0 |	app  |	ota_0 |	0x10000 |	0x140000 	
app1 |	app |	ota_1 |	0x150000 |	0x140000 	
spiffs |	data |	spiffs |	0x290000 |	0x170000 	


The ota data is only 8,192 bytes
-->


## 3. Solutions in the Arduino Environment

There is no possibility to change the partition scheme in the IDE, contrary to other ESP32 board definitions. One work around is to modify the value of the `esp32cam.build.partition` setting in the `boards.txt` file. It can be found in the `.../packages/esp32/hardware/esp32/2.x.x` directory. It will be possible to run the `BasicOTA` sketch with values such as `default` and `min_spiffs`.

That solution would certainly work, but it would be necessary to edit the board definition file each time the `arduino-esp32` core is updated.

The other solution is to select a different board in the `Tools` menu of Arduino IDE. The following choice works.
```
   |  Board: "ESP32 Wrover Kit (all versions)"
   |  Upload Speed: "921600"
   |  CPU Frequency: "240 MHz (WiFi/BT)"
   |  Flash Frequency: "80MHz"
   |  Flash Mode: "QIO"
   |  Flash Size: "4MB (32Mb)"
   |  Partition Scheme: "Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS")
   |  Core Debug Level: "None"
   |  PSRAM: "Enabled"
```
Once `ESP32 Wrover Kit (all versions)` is selected all the other values shown above are default values. Three partitions schemes would enable OTA:

```
   1. "Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS"
   2. "Default 4MB with ffat (1.2MB APP/1.5MB FATFS"
   3. "Minimal SPIFFS (1.9MB APP/190KB SPIFFS"
``` 
This is certainly the simplest approach. However, there is no assurance that it would work with all the ESP32-CAM clones currently available although the `esp32wroverkit` settings in `boards.txt` look generic enough.

## 4. Solutions in the PlatformIO Environement

The two solutions discussed in the previous section would certainly work with PIO. The directory containing the `boards.txt` file will be different, of course. On the author's Linux system, it is `~/.platformio/packages/framework-arduinoespressif32`. 

To use a different board definition, then the change must be made in the `platformio.ini` file. 

```ini
[env:esp32cam]
platform = espressif32
board = esp32cam
```

could be replaced with 

```ini
[env:esp-wrover-kit]
platform = espressif32
board = esp-wrover-kit
```

The following substitution

```ini
[env:esp32cam]
platform = espressif32
board = esp-wrover-kit
```

would probably work just as well with the added advantage of confusing everyone.

However, the simplest solution in PlatformIO is to explicitly specify the partition scheme to use.

```ini
[env:esp32cam]
platform = espressif32
board = esp32cam
board_build.partitions = min_spiffs.csv  ; needed for OTA
```

## 5. Simplifying the use of ArduionoOTA

The `ota.h` and `ota.cpp` files extract most of the OTA code from original `BasicOTA.ino` source to make it easier to add "push" OTA in other projects. This idea is ~~stolen~~ borrowed from Andreas Spiess (*The guy with the Swiss accent* on YouTube and *SensorsIot* on GitHub. The use of the *credentials* file to define the Wi-Fi settings has been extended to include the OTA settings. 

The firmware of the ESP32 must be modified to enable OTA using ArduinoOTA. Include the `ota.h` and `ota.cpp` files in the project, add a call to `setupOTA()` with the proper parameters in the sketch `setup()` function and add a call to ` handleOTA()` in the `loop()` function.

If **FreeRTOS** is to be used or if OTA debugging is desired, please consult the [original Andreas Spiess code](https://github.com/SensorsIot/ESP32-OTA). 


## 6. Last Comments and Instructions

This .INO file is nothing but a long comment but it nevertheless satisfies the Arduino requirements.
The true source of the project is in the file `main.cpp`. 

**Do not forget** to edit the settings in `secrets-template.h` file and to save it as `secrets.h`. 

If working in the Arduino programming environment, copy the `ESP32_CAM_OTA` directory to the sketchbook directory. The sibling directories, `include`, `lib` and `test` as well as the other files at the same level including this file are not needed.

The `upload_port` and `upload_flags` settings in `platformio.ini` must be edited to match the values in `secrets.h` if using the PlatformIO programming environment is used.

## 7. License
 
Since this project is clearly derivative work, it can be assumed that the license under which Espressif released **BasicOTA** applies. However there is no copyright notice in that file. The only mention of a license is in the root directory of the [Arduino core for the ESP32, ESP32-S2 and ESP32-C](https://github.com/espressif/arduino-esp32) repository containing the four source files. There it states that the *LGPL-2.1 License* applies.  

As for any original work found in this repository, it is released under the **BSD Zero Clause** ([SPDX](https://spdx.dev/): [0BSD](https://spdx.org/licenses/0BSD.html)) licence.

(c) Copyright, 2022, Michel Deslierres
