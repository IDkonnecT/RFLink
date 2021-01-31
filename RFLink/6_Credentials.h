// ************************************* //
// * Arduino Project RFLink-esp        * //
// * https://github.com/couin3/RFLink  * //
// * 2018..2020 Stormteam - Marc RIVES * //
// * More details in RFLink.ino file   * //
// ************************************* //

#ifndef CREDENTIALS_h
#define CREDENTIALS_h

#include "RFLink.h"


String FiltreMac(String Mac)
{
  String NoSerie;
  NoSerie.reserve(Mac.length()); // optional, avoids buffer reallocations in the loop
  for(size_t i = 0; i < Mac.length(); ++i) if(Mac[i] != ':') NoSerie += Mac[i];
  return NoSerie;
}

#ifdef WIFIMANAGER_ENABLED
// Device
const char* DeviceName = "IDkonnecT";

// WiFiManager AP
const char* WiFiManager_SSID = DeviceName;
IPAddress WiFiManager_IP = IPAddress(192,168,1,1);
IPAddress WiFiManager_Gateway = IPAddress(192,168,1,1);
IPAddress WiFiManager_SubNet = IPAddress(255,255,255,0);
#endif

// FOTA file http address
#ifdef FOTA_ENABLED
String FOTA_FILE = "http://www.idkonnect.fr/RFLink/firmware.bin";
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
String MQTT_SERVER = "www.idkonnect.fr";
String MQTT_PORT = "1883"; // Devrait marcher avec : "8080" !!! Est-ce le mode SSL ???
String MQTT_ID = "RFLink_"+FiltreMac(WiFi.macAddress());
String MQTT_USER = "RFLink_"+FiltreMac(WiFi.macAddress());
String MQTT_PSWD = "";

// MQTT Topic
String MQTT_TOPIC_OUT = "msg/RFLink/"+FiltreMac(WiFi.macAddress());
String MQTT_TOPIC_IN = "cmnd/RFLink/"+FiltreMac(WiFi.macAddress())+"/#"; // Must end with '/#' to read eQ3 commands
String MQTT_TOPIC_LWT = "msg/RFLink/"+FiltreMac(WiFi.macAddress())+"/LWT";

#endif