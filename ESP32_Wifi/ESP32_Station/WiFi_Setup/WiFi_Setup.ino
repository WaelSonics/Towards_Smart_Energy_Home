#include <Arduino.h>
#include <WiFi.h>

#define WIFI_Network "INTERNET - 71189805 (009)"
#define WIFI_Password "60708090"
#define WIFI_Timeout 20000

void ConnectToWifi(){
  Serial.print("Connecting to Wifi:");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_Network, WIFI_Password);
  unsigned long Start = millis();
  while (WiFi.status() != WL_CONNECTED && (millis()-Start) < WIFI_Timeout){
  Serial.print(".");
  delay(200);
 }
 if(WiFi.status()!= WL_CONNECTED){
  Serial.print("Failed To Connect!!!");
 }
 else {
  Serial.print("Connected Successfuly\n");
  Serial.print(WiFi.localIP());
 }
}

void setup() {
  Serial.begin(9600);
  ConnectToWifi();
}

void loop() {
}
