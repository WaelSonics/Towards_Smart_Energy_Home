#include <WiFi.h>                                   
#include <WebServer.h>                               
#include <WebSocketsServer.h>                         

const char* ssid = "INTERNET - 71189805 (009)";
const char* password = "60708090";

String website = "<!DOCTYPE html><html><head><title>Page Title</title></head><body style='background-color: #EEEEEE;'><span style='color: #003366;'><h1>Towards Smart Homes</h1><p>The Power sent from the ESP32 is for example: <span id='rand'>-</span></p><p><button type='button' id='BTN_SEND_BACK'>Send info to ESP32</button></p></span></body><script> var Socket; document.getElementById('BTN_SEND_BACK').addEventListener('click', button_send_back); function init() { Socket = new WebSocket('ws://' + window.location.hostname + ':81/'); Socket.onmessage = function(event) { processCommand(event); }; } function button_send_back() { Socket.send('Command Recieved From a Client Still DK WHO'); } function processCommand(event) { document.getElementById('rand').innerHTML = event.data; console.log(event.data); } window.onload = function(event) { init(); }</script></html>";

int interval = 1000;                                  // send data to the client every 1000ms
unsigned long previousMillis = 0;                     

WebServer server(80);                                 
WebSocketsServer webSocket = WebSocketsServer(81);    

void setup() {
  Serial.begin(9600);                         
  WiFi.begin(ssid, password);                         
  Serial.println("Establishing connection to WiFi with SSID: " + String(ssid));    
  while (WiFi.status() != WL_CONNECTED) {             
    delay(1000);
    Serial.print(".");
  }
  Serial.print("Connected to network with IP address: ");
  Serial.println(WiFi.localIP());                     // show IP address that the ESP32 has received from router
  
  server.on("/", []() {                               // define here wat the webserver needs to do
    server.send(200, "text\html", website);           //    -> it needs to send out the HTML string "webpage" to the client
  });
  server.begin();                                     // start server
  
  webSocket.begin();                                  // start websocket
  webSocket.onEvent(webSocketEvent);                  
}

void loop() {
  server.handleClient();                              // Needed for the webserver to handle all clients
  webSocket.loop();                                   // Update function for the webSockets 
  
  unsigned long now = millis();                      
  if ((unsigned long)(now - previousMillis) >= interval) 
  {  
    String str = String(random(100));               
    int str_len = str.length() + 1;                   
    char char_array[str_len];
    str.toCharArray(char_array, str_len);             // convert to char array
    webSocket.broadcastTXT(char_array);               // send char_array to clients
    previousMillis = now;                             // reset previousMillis
  }
}

void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length) {      // the parameters of this callback function are always the same -> num: id of the client who send the event, type: type of message, payload: actual data sent and length: length of payload
  switch (type) {                                    
    case WStype_DISCONNECTED:                         
      Serial.println("Client " + String(num) + " disconnected");
      break;
    case WStype_CONNECTED:                           
      Serial.println("Client " + String(num) + " connected");
      break;
    case WStype_TEXT:                                 
      for (int i=0; i<length; i++) {                
        Serial.print((char)payload[i]);
      }
      Serial.println("");
      break;
  }
}
