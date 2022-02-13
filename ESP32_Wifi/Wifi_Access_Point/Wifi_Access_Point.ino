#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
 
#define LED_BUILTIN 2   
 
const char *ssid = "Wael_ESP32";
const char *password = "CRonaldo";
 
WiFiServer server(80);
 
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  delay(3000);
  Serial.println();
  Serial.println("Configuring access point...");
  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  server.begin();
  Serial.println("Server started");
}
 
void loop() {
  WiFiClient client = server.available();  
 
  if (client) {                             
    Serial.println("New Client.");        
    String currentLine = "";              
    while (client.connected()) {            
      if (client.available()) {            
        char c = client.read();            
        Serial.write(c);                   
        if (c == '\n') {                   
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");
            client.println();
            break;
          } 
		  else {   
            currentLine = "";
          }
        } 
		  else if (c != '\r') {  
			currentLine += c;     
        }
		
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_BUILTIN, HIGH);           
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, LOW);          
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
