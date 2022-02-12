#include <ESP8266WiFi.h>
 
#define WIFI_SSID "INTERNET - 71189805 (009)"
#define WIFI_PASS "60708090"

#define AP_SSID "WAEL_AP_ST"
#define AP_PASS "WaelSonics"
 
void setup()
{
  Serial.begin(115200);
  Serial.println();
 
  // Begin Access Point
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(AP_SSID, AP_PASS);
 
  // Begin WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  delay(5000);

  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.println(".");
  }
  Serial.println("Connected!");
  Serial.print("IP address for network ");
  Serial.print(WIFI_SSID);
  Serial.print(" : ");
  Serial.println(WiFi.localIP());
  Serial.print("IP address for network ");
  Serial.print(AP_SSID);
  Serial.print(" : ");
  Serial.print(WiFi.softAPIP());
 
}
 
void loop() {
  
}
