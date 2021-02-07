# 1 "C:\\Users\\J490033\\AppData\\Local\\Temp\\tmpe32gpuhd"
#include <Arduino.h>
# 1 "C:/User/J490033/Documents/! PERSO/! RFkonnecT/RFkonnecT.git/RFLink/RFLink.ino"
# 19 "C:/User/J490033/Documents/! PERSO/! RFkonnecT/RFkonnecT.git/RFLink/RFLink.ino"
#include <Arduino.h>
#include "RFLink.h"
#include "1_Radio.h"
#include "2_Signal.h"
#include "3_Serial.h"
#include "4_Display.h"
#include "5_Plugin.h"
#include "6_WiFi_MQTT.h"
#include "8_OLED.h"


#include "9_WiFiManager_FOTA.h"
#include "eQ3Thermostat.h"


#if (defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__))
#include <avr/power.h>
#endif

void sendMsg();

#if (defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__))
void (*Reboot)(void) = 0;
void CallReboot(void);
void CallReboot(void);
void setup();
void loop();
#line 43 "C:/User/J490033/Documents/! PERSO/! RFkonnecT/RFkonnecT.git/RFLink/RFLink.ino"
void CallReboot(void)
{
  sendMsg();
  delay(1);
  Reboot();
}
#endif

#if (defined(ESP8266) || defined(ESP32))
void CallReboot(void)
{
  sendMsg();
  delay(1);
  ESP.restart();
}
#endif

void setup()
{
#if (defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__))

  ADCSRA = 0;
  power_all_disable();
  power_timer0_enable();
  power_usart0_enable();
  delay(250);
#ifdef RFM69_ENABLED
  power_spi_enable();
#endif
#elif defined(ESP32)
  btStop();
#endif

  delay(250);
  Serial.begin(BAUD);

#if (defined(ESP32) || defined(ESP8266))
  Serial.println();
  Serial.print(F("Arduino IDE Version :\t"));
  Serial.println(ARDUINO);
#ifdef ESP8266
  Serial.print(F("ESP CoreVersion :\t"));
  Serial.println(ESP.getCoreVersion());
#endif
  Serial.print(F("Sketch File :\t\t"));
  Serial.println(__FILE__);
  Serial.println(F("Compiled on :\t\t" __DATE__ " at " __TIME__));

#ifdef MQTT_ENABLED

#ifndef WIFIMANAGER_ENABLED
  setup_WIFI();
  start_WIFI();
#else
  pinMode(PIN_RST_WIFIMANAGER, INPUT_PULLUP);
  SetUpWiFiManager();
#endif

  setup_MQTT();
  reconnect();
#else
  setup_WIFI_OFF();
#endif
#endif

  PluginInit();
  PluginTXInit();
  set_Radio_mode(Radio_OFF);

#if ((defined(ESP8266) || defined(ESP32)) && !defined(RFM69_ENABLED))
  show_Radio_Pin();
#endif

#ifdef OLED_ENABLED
  setup_OLED();
#endif

  display_Header();
  display_Splash();
  display_Footer();

#ifdef SERIAL_ENABLED
  Serial.print(pbuffer);
#endif
#ifdef OLED_ENABLED
  splash_OLED();
#endif
#ifdef MQTT_ENABLED
  publishMsg();
#endif
  pbuffer[0] = 0;
  set_Radio_mode(Radio_RX);

#ifdef EQ3THERMOSTAT
  SetupThermostats();
  ScanThermostats(20);
#endif

}

void loop()
{

#ifdef WIFIMANAGER_ENABLED
  if (digitalRead(PIN_RST_WIFIMANAGER)==LOW)
  {
    set_Radio_mode(Radio_OFF);
    WiFiManagerPortal();
  }
#endif
#ifdef EQ3THERMOSTAT

  int Periodicite = 1 + millis()/600000;
  if ((millis()/1000)%(60*Periodicite)==0) ScanThermostats(10);
#endif

#ifdef MQTT_ENABLED
  checkMQTTloop();
  sendMsg();
#endif

#ifdef SERIAL_ENABLED
#if PIN_RF_TX_DATA_0 != NOT_A_PIN
  if (CheckSerial())
    sendMsg();
#endif
#endif
  if (ScanEvent())
    sendMsg();
}

void sendMsg()
{
  if (pbuffer[0] != 0)
  {
#ifdef SERIAL_ENABLED
    Serial.print(pbuffer);
#endif
#ifdef MQTT_ENABLED
    publishMsg();
#endif
#ifdef OLED_ENABLED
    print_OLED();
#endif
    pbuffer[0] = 0;
  }
}