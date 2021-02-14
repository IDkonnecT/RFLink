# Fork of RFLink ESP

This is an RF to MQTT gateway build for an ESP32 board. 

This is a Fork of https://github.com/couin3/RFLink, with some modifications.

It receive OOK 433MHz signals, then it identifies, decodes and push them over MQTT.
The Projet is based on RFlink project "R29" (see rflink.nl, latest known open source shard version).

![RFkonnecT](https://github.com/IDkonnecT/RFkonnecT/blob/master/pictures/RFkonnecT.jpg "RFkonnecT")

## 1. MCU
I use extensively ESP32, but code should also work for ESP8266.

## 2. Receiver / Transmitter / Transceiver
I mainly use RFM69 modules for RF433, but other modules also work.
I designed my own PCB for better integration.

![PCB](https://github.com/IDkonnecT/RFkonnecT/blob/master/pictures/RFkonnecT.jpg "PCB")

## 3. OLED display
You can use an OLED display, but I do not personnaly, so didn't do any testing.
Master branch use SSD1306 128x64 I2C screen for testings.
*** This is highly experimental ***, and thus not activated by default.
## 4. IDE
- We strongly recommend using PlatformIO IDE (https://platformio.org/install)
- You may alternatively use Arduino IDE 1.8.10 (https://www.arduino.cc/en/Guide/HomePage)

## 5. Framework
We use Arduino Core for ESP32 https://github.com/espressif/arduino-esp32

## 6. Libraries
So far, in addition of core libraries, we use:
- PubSubClient for MQTT messaging https://github.com/knolleary/pubsubclient
- WiFi Manager for WiFi configuration https://github.com/tzapu/WiFiManager.git#development
- u8g2/u8x8 library for OLED display https://github.com/olikraus/u8g2

## 7. COMPILE OPTIONS AND FLAGS
Many features are not enabled by default or can be disabled for various reasons : firmware size, compability etc etc. Here is a listing with some instructions:
### MQTT Server (disabled by default)
- Define compilation flag MQTT_ENABLED or define it in 6_WIFI_MQTT.h
- If you have not enabled WifiManager, make sure you update Wifi settings in 6_Credentials.h

## 8. Additional info
### Pinout
!!! To be updated !!!


|  Name         | D1 mini | RXB6  |
|---------------|---------|-------|
| PIN_RF_TX_VCC |   D5    | 5 VCC |
| PIN_RF_TX_NA  |   D6    | 6 DER |
| PIN_RF_TX_DATA|   D7    | 7 DAT |
| PIN_RF_TX_GND |   D8    | 8 GND |



### Thanks
Special thanks to: Couin3, Axellum, Etimou, Schmurtz, Zoomx 
