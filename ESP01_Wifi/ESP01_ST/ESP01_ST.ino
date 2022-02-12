#include <ESP8266WiFi.h>

#define WIFI_Network "INTERNET - 71189805 (009)"
#define WIFI_Password "60708090"

void ConnectToWifi(){
  delay(5000);
  Serial.print("Connecting to Wifi:");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_Network, WIFI_Password);
  while(WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(100);
    }
  Serial.print("Connected Successfuly\n");
  Serial.print("Local IP: ");
  Serial.print(WiFi.localIP());
}

void setup() {
  Serial.begin(9600);
  ConnectToWifi();
}

void loop() {
}
