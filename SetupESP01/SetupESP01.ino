#include <EEPROM.h>  
#include <ESP8266WiFi.h>                                   
#include <ESP8266WebServer.h>          
#include <ArduinoJson.h>                     
#include <WebSocketsServer.h>


const int wsMsgCap = JSON_OBJECT_SIZE(4);
DynamicJsonDocument wsJson(wsMsgCap);

bool OperationModeBool = false;
bool SetupModeBool = true;                  //booleans for mode
bool EraseEEpromBool = false;              //deleted when reset button entered
bool submitBool = false;

struct settings {                     //structure for wifi credentials to be taken from user (from HTTP)
  char ssid[30];
  char password[30];
} user_wifi = {};

enum CONX_STATUS{CONX, NO_CONX};                  		//Message Sent or Recieved
enum WS_MSG_STATUS{MSG_SENT, MSG_RECIEVED};             //Setup Station State Sent=0, Recieved=1

enum WS_MSG_DEVICE{DEVICE_RELAY,DEVICE_APP};            //dk yet


ESP8266WebServer server(80);                  //initialize webserver   
WebSocketsServer webSocket = WebSocketsServer(81);     //initialize websocket server

char website[] = 
R"=====(

<!doctype html>
<html lang='en'>
   <head>
      <meta charset='utf-8'>
      <meta name='viewport' content='width=device-width, initial-scale=1'>
      <title>Wifi Setup</title>
      <style>
         *,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{cursor: pointer;border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1{text-align: center}
      </style>
   </head>
   <body>
      <main class='form-signin'>
      
           <form action='/' method='post'>
              <h1 class=''>Wifi Setup</h1>
              <div class='form-floating'><label>SSID</label><input type='text' class='form-control' id='ssid' name='ssid'> </div>
              <div class='form-floating'><br/><label>Password</label><input type='password' class='form-control'id='password' name='password'></div>
           </form>
           <br/><br>
           <button id="submitButton" class="button" onclick="sendData();">Submit Data</button>         
           <br/><br>
           <center><textarea id='feedback' name='feedback' rows='15' cols='40'>
            Feedback From ESP32:
           </textarea></center>
         
       </main>
         
  </body>
   <script>
   
   var ssid;
   var pass;
   var wifi_feedback;
   var Socket;
   Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
   const WS_STATUS = {
    SENT : 1,
    RCVD : 0
  };
  const WS_CONX_FEEDBACK = {
    CONX : 0,
    NO_CONX : 1
  };

   function sendData() {
    ssid = document.getElementById('ssid');
    pass = document.getElementById('password');
    Socket.send(JSON.stringify({"ssid":ssid.value,"pass":pass.value}));
   }
   
   Socket.onmessage = function(event) {
    var obj = JSON.parse(event.data);
  if (obj.status == WS_STATUS.RCVD && obj.connection == WS_CONX_FEEDBACK.CONX){
  var local_IP = obj.ip;
    wifi_feedback = "\n Connected to Wifi with local IP: " + local_IP;
    document.getElementById("feedback").value += wifi_feedback;
  }
  else if (obj.status == WS_STATUS.RCVD && obj.connection == WS_CONX_FEEDBACK.NO_CONX){
    wifi_feedback = "\n Failed To Connect, PLEASE Retry...";
    document.getElementById("feedback").value += wifi_feedback;
  }
  else{
    wifi_feedback = "\n nor a sent neither w";
    document.getElementById("feedback").value += wifi_feedback;
  }
   };
   
</script>
</html>

)=====";



void EraseEEPROM(){
  for (int i = 0; i < 512; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  delay(500);
  Serial.println("EEPROM Erased...");
}

void SetupModeON() {                             //Setup Function

  EEPROM.begin(sizeof(struct settings));         //EEPROM for taking and saving
  EEPROM.get( 0, user_wifi );
  WiFi.mode(WIFI_STA);						     //Station Mode if wifi credentials already saved in eeprom
  WiFi.begin(user_wifi.ssid, user_wifi.password);

  byte tries = 0;                     //Try 10 Times to connect
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (tries++ > 10) {                   //If failed Start Access Point to give html page to user
      WiFi.mode(WIFI_AP_STA);             //AP_Staion mode for setup
      WiFi.softAP("WaelSetup", "waelsetup");
      Serial.println("Access Point To Get Wifi Credentials...");
      Serial.write("Soft-AP IP address = ");
      Serial.println(WiFi.softAPIP());            //IP address by default: 192.168.4.1 (can be changed)

      break;
      }
  }
  if (WiFi.status() != WL_CONNECTED){
      server.on("/", []() {                               // define here what the webserver needs to do
       server.send(200, "text\html", website);           // it needs to send out the HTML string "webpage" to the client
      });
      server.begin();                                     // start server
      Serial.println("Servers Started...");
      webSocket.begin();                                  // start websocket
      webSocket.onEvent(webSocketEvent);
  }
  else {
    OperationModeBool = true;
    Serial.println("Operation Mode Turned ON bbbbbbbbbbb");
  }
}

bool JsonDecodeWsMsg(byte* data){
    wsJson.clear();
    DeserializationError error = deserializeJson(wsJson, data);
    // Test if parsing succeeds.
    if (error) {
      //Serial.write(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return false;
    }
   return true;
  }
  
const char* JsonStringifyWsMsgSetup(byte wsStatus,byte conx, const char* data_ip){
  wsJson.clear();
  wsJson["device"] = DEVICE_RELAY;
  wsJson["status"] = wsStatus;
  wsJson["connection"] = conx;
  wsJson["ip"] = data_ip;
  
  String strMsg;
  serializeJson(wsJson, strMsg);
  char* str=(char*)malloc(sizeof(char)*strMsg.length()+1);
  strMsg.toCharArray(str,(strMsg.length()+1));
  return str;
}

void CopyCredentialsTaken(){              //function to copy data to local "user_wifi"
  strncpy(user_wifi.ssid, wsJson["ssid"], sizeof(user_wifi.ssid));
  Serial.println("ssid: " + String(user_wifi.ssid));
  strncpy(user_wifi.password, wsJson["pass"], sizeof(user_wifi.password));
  Serial.println("pass: " + String(user_wifi.password));
  submitBool = true;                  //submit button is pressed
}
 
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:                         
      Serial.println("Client " + String(num) + " disconnected");
      break;
    case WStype_CONNECTED:                           
      Serial.println("Client " + String(num) + " connected");
      break;
    case WStype_TEXT:                     
    {
      //just for degub
      Serial.write("the string payload:");
      for (int i=0; i<length; i++)                
        Serial.write((char)payload[i]);
      Serial.println();
      
      //decode to retreive data 
      JsonDecodeWsMsg(payload);
    CopyCredentialsTaken();
    
     } break;
  }
}

void CheckWifiCredentials(){                //function to check if the esp is connected to wifi after submit button
  if(WiFi.status()!= WL_CONNECTED){
    WiFi.begin(user_wifi.ssid ,user_wifi.password);
    unsigned long Start = millis();
    while (WiFi.status() != WL_CONNECTED && (millis()-Start) < 7000){
      Serial.write(".");
      delay(200);
    }
  }

  if(WiFi.status()!= WL_CONNECTED){
    Serial.println("\nFailed To Connect!!!");
    String w = "w";
    webSocket.broadcastTXT(JsonStringifyWsMsgSetup(MSG_SENT, NO_CONX, (w).c_str()));
  }
  else {
    Serial.write("\nConnected Successfuly with IP Address:\n");
    Serial.println(WiFi.localIP());
    webSocket.broadcastTXT(JsonStringifyWsMsgSetup(MSG_SENT, CONX, (WiFi.localIP().toString()).c_str()));    //IP address should be sent to html page
    EEPROM.put(0, user_wifi);
    EEPROM.commit();
  }
}



void setup() {
  Serial.begin(9600);
}


void loop() {
    server.handleClient();  
    webSocket.loop();             // Update function for the webSockets

  if (EraseEEpromBool){             //Reset EEPROM when reset button is clicked
    EraseEEPROM();
    EraseEEpromBool = false;
    delay(1000);
    }
  
  if (SetupModeBool){                              //If setup mode (by default it starts as setup)
    Serial.println("Setup Mode ON...");        
    SetupModeON();                                //Call setup fn
    SetupModeBool = false;                        //Once called no need to repeat since in the setup function it waits for input and tries to connect to wifi
  }
    
  if (submitBool){          //if submit button (in setup) is pressed go check station conx 
  submitBool = false;       //to do this once for each press
  CheckWifiCredentials();
  }
  
  if((WiFi.status() == WL_CONNECTED) && (OperationModeBool == false)){			//If connected to wifi for 1st time
	  Serial.println("Operation Mode Started!!!");
	  delay(5000);
    WiFi.mode(WIFI_OFF);								//Turn Off Access Point 
    delay(100);
    WiFi.mode(WIFI_STA);								//disconnect from the webserver used for setup mode
    WiFi.begin(user_wifi.ssid, user_wifi.password);		//reconnect to the wifi
    unsigned long Start = millis();
	  while (WiFi.status() != WL_CONNECTED && (millis()-Start) < 7000){
      Serial.write(".");
      delay(200);
    }
	  if(WiFi.status() == WL_CONNECTED){
	    OperationModeBool = true;							//Turn Into Operation Mode
	    Serial.println("\nConnected Successfully Again to your Network! :-)");
	  }
	  else{
	    Serial.println("\nFailed to continue with Operation Mode (Setup Mode Restarted)!!");
	    SetupModeBool = true; 							//restart the setup mode
	  }
  }

  if((WiFi.status() == WL_CONNECTED) && (OperationModeBool == true)){			//If connection succeeds initialize Operation Mode
    //OperationModeON();
    Serial.println("Operation MODE BROOO Finally :-)");
    Serial.write("Local IP: ");
    Serial.println(WiFi.localIP());                     				//print station IP of esp
    delay(3000);
  }
}
