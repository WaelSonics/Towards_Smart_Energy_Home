#include <WiFi.h>

const char* wifi_network_ssid     = "INTERNET - 71189805 (009)";
const char* wifi_network_password =  "60708090";

const char *soft_ap_ssid          = "WAELESP32";
const char *soft_ap_password      = NULL;

void setup()
{
    Serial.begin(9600);
    WiFi.mode(WIFI_AP_STA);
    delay(9000);
    Serial.println("\n[*] Creating ESP32 AP");
    WiFi.softAP(soft_ap_ssid, soft_ap_password);
    Serial.print("[+] AP Created with IP Gateway ");
    Serial.println(WiFi.softAPIP());

    WiFi.begin(wifi_network_ssid, wifi_network_password);
    Serial.println("\n[*] Connecting to WiFi Network");
    Serial.println("BEFORE SWITCH...");
    delay(2000);
    Serial.println("AFTER SWITCH...");
    while(WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(100);
    }

    Serial.print("\n[+] Connected to the WiFi network with local IP: ");
    Serial.println(WiFi.localIP());
}

void loop() {}
