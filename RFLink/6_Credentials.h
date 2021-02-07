// ************************************* //
// * Arduino Project RFLink-esp        * //
// * https://github.com/couin3/RFLink  * //
// * 2018..2020 Stormteam - Marc RIVES * //
// * More details in RFLink.ino file   * //
// ************************************* //

// Replace Credentials.h by this file (by renaming it)

#ifndef CREDENTIALS_h
#define CREDENTIALS_h

#include "RFLink.h"

#if __has_include("6_Credentials_Perso.h")
#include "6_Credentials_Perso.h"
#else

#ifdef WIFIMANAGER_ENABLED
// Device
const char* DeviceName = "my_RFLink";

// WiFiManager AP
const char* WiFiManager_SSID = DeviceName;
IPAddress WiFiManager_IP = IPAddress(192,168,1,1);
IPAddress WiFiManager_Gateway = IPAddress(192,168,1,1);
IPAddress WiFiManager_SubNet = IPAddress(255,255,255,0);
#endif

// FOTA file http address
#ifdef FOTA_ENABLED
String FOTA_FILE = "http://www.monsite.fr/RFLink/firmware.bin";
#endif


// local AP
String WIFI_SSID = "your_AP_ssid";
String WIFI_PSWD = "your_AP_passwd";

// DHCP or Static IP
#define USE_DHCP
#ifndef USE_DHCP
String WIFI_IP = "192.168.0.199";
String WIFI_DNS = "8.8.8.8";
String WIFI_GATEWAY = "192.168.0.1";
String WIFI_SUBNET = "255.255.255.0";
#endif

// MQTT Server
String MQTT_SERVER = "raspberrypi.local";
String MQTT_PORT = "1883";
String MQTT_ID = "ESP8266-RFLink";
String MQTT_USER = "your_mqtt_user";
String MQTT_PSWD = "your_mqtt_pswd";

// MQTT Topic
String MQTT_TOPIC_OUT = "/RFLink/msg";
String MQTT_TOPIC_IN = "/RFLink/cmd/#"; // Must end with '/#' to read eQ3 commands
String MQTT_TOPIC_LWT = "/RFLink/lwt";

#endif // if not __has_include("6_Credentials_Perso.h")
#endif // CREDENTIALS_h