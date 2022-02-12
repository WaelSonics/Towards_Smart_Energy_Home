#include <ESP8266WiFi.h>

void setup()
{
  Serial.begin(115200);
  Serial.println();
  delay (3000);
  Serial.print("Setting soft-AP ... ");
  WiFi.softAP("ESP01_Wael") ? "Ready" : "Failed!";
}


void loop()
{
  Serial.printf("Stations connected = %d,  Local IP Address: %d\n", WiFi.softAPgetStationNum(), WiFi.softAPIP());
  delay(7000);
}
