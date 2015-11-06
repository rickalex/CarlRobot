/*
  
The MIT License (MIT)

Copyright (C) 2015, Miguel Medeiros, Dominic Phillips

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.


*/

// Include Librairies
  #include <ESP8266WiFi.h>
  #include <WiFiClient.h>
  #include <ESP8266WebServer.h> 
  #include "websites.h"
// Include Librairies End


// IO Extender
  #include <Wire.h>  // Wire.h library is required to use SX1509 lib
  #include <sx1509_library.h>  // Include the SX1509 library
  const byte SX1509_ADDRESS = 0x3E;  // SX1509 I2C address (00)
  const byte interruptPin = 2; // not used yet
  sx1509Class sx1509(SX1509_ADDRESS);
   
  #define mot1PinA 0 // Brown A-IA  DO on nodemcu
  #define mot1PinB 1  //Red  A -IB   D1 on nodemcu
  #define mot2PinA 2  // orange  B - IA  = D2 
  #define mot2PinB 3  // yellow  B-IB = D3
// IO Extender END

// Start ESP8266 WebServer
  ESP8266WebServer server(80);

  // Starts the ESP8266 Web Server
// Start ESP8266 WebServer END


// Variables

  // General Variables
    bool Robot_Motor_Moving = false; // If the Robot is Moving
    unsigned int Robot_Motor_Speed = 230; // Forward and Reverse Motor Speed
    unsigned int Robot_Motor_TurnSpeed = 190; // Left and Right Turning Speed



    #define mot1PinA 0 // Brown A-IA  DO on nodemcu
    #define mot1PinB 1  //Red  A -IB   D1 on nodemcu
    #define mot2PinA 2  // orange  B - IA  = D2 
    #define mot2PinB 3  // yellow  B-IB = D3
  // General Variables END

  // Web Page
    const char Robot_Web_Page[] = "<html>"\
    "<head>"\
    "<title>Robot Control</title>"\
    "<style>"\
              "html, body {width:85%;margin:0 auto;padding:0;background-color:#cfcbd1;text-align:center;}"\
              ".f{display:block;}"\
              ".rev{display:block;}"\
              ".l{display:inline;}"\
              ".r{display:inline;}"\
              ".s{display:inline;}"\
              ".fb{vertical-align:middle;width:150px;height:120px;margin:25px;background-color:#45cc5a;font-size:20px}"\
              ".revb{vertical-align:middle;width:150px;height:120px;margin:25px;background-color:#cc45b7;font-size:20px}"\
              ".lb{vertical-align:middle;width:150px;height:120px;margin:25px;background-color:#4574cc;font-size:20px}"\
              ".rb{vertical-align:middle;width:150px;height:120px;margin:25px;background-color:#cc9d45;font-size:20px}"\
              ".sb{vertical-align:middle;width:150px;height:120px;margin:25px;font-size:20px}"\
    "</style>"\
    "</head>"\
    "<body>"\
        "<h1>Click Button to Redirect</h1><br><br><br>"\
        "<form class=\"f\" action=\"http://mine-teamdman.c9.io/nips.html\"> <input type=\"submit\" class=\"fb\" value=\"Redirect\">  </form>" \
    "</body>"\
    "</html>";

    void handleRoot(){
      //server.send (200, "text/html", loadFromFlash("index"));
      Serial.println("TEST");
    }
  // Web Page END

  // Wireless Chip Configuration
    char Robot_Wireless_SSID[] = "OCSB";     //  Wireless Network Name (name) 
    char Robot_Wireless_PASS[] = "wireless4all";  // Wireless Network Password
    int Robot_Wireless_Status = WL_IDLE_STATUS;     // the Wifi radio's status
  // Wireless Chip Configuration END
// Variables End










void setup() {
  // SX1509 Setup
    sx1509.init();  // Initialize the SX1509, does Wire.begin()
    byte clockDivider = 1;
    sx1509.ledDriverInit(mot1PinA);
    sx1509.ledDriverInit(mot1PinA, clockDivider, LOGARITHMIC);
    sx1509.ledDriverInit(mot1PinB);
    sx1509.ledDriverInit(mot1PinB, clockDivider, LOGARITHMIC);
    sx1509.ledDriverInit(mot2PinA);
    sx1509.ledDriverInit(mot2PinA, clockDivider, LOGARITHMIC);
    sx1509.ledDriverInit(mot2PinB);
    sx1509.ledDriverInit(mot2PinB, clockDivider, LOGARITHMIC);
  // SX1509 Setup END

  // Serial Setup
   Serial.begin(9600); 
    while (!Serial) {
      ; // wait for serial port to connect. Needed for Leonardo only
    }
  // Serial Setup END

  // Check if WiFi Shield Available
    if (WiFi.status() == WL_NO_SHIELD) {
      Serial.println("WiFi shield not present"); 
      // don't continue:
      while(true);
    }
  // Check if WiFi Shield Available END

  // Start Wireless Connection
      WiFi.mode(WIFI_STA);
      WiFi.begin(Robot_Wireless_SSID, Robot_Wireless_PASS);
  // Start Wireless Connection END

  // Wait for Wireless Connection
    while ( WiFi.status() != WL_CONNECTED) {
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(Robot_Wireless_SSID);
        delay(1000);
      }
  // Wait for Wireless Connection END


  // On Connection
    printCurrentNet();
    printWifiData();
   // server.on("/", handleRoot);
    server.on("/Forward", Forward);
    server.on("/ForwardLeft", ForwardLeft);
    server.on("/ForwardLeftSharp", ForwardLeftSharp);
    server.on("/ForwardRight", ForwardRight);
    server.on("/ForwardRightSharp", ForwardRightSharp);
    server.on("/Reverse", Reverse); 
    server.on("/Left", Left);
    server.on("/Right", Right); 
    server.on("/Stop", Stop);
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("Webserver: Server Started!");
  // On Connection END

}






void loop() {
  server.handleClient(); 
}







// Left Right Code Parsing
  void Left() {
    Serial.println("Robot: Received Go Left");
      sx1509.pwm(mot1PinA, Robot_Motor_TurnSpeed);
      sx1509.pwm(mot1PinB, 0);


      sx1509.pwm(mot2PinA, 0);
      sx1509.pwm(mot2PinB, Robot_Motor_TurnSpeed);
      //handleRoot();
      delay(140);
      /*if (Robot_Motor_Moving == true) {
        Forward();
      } else {
        Stop();
      }
      */
  }

  void ForwardLeft() {
    Serial.println("Robot: Received Go Left while moving forward");
      sx1509.pwm(mot1PinB, (Robot_Motor_Speed - 20));
      sx1509.pwm(mot1PinA, 0);


      sx1509.pwm(mot2PinB, Robot_Motor_Speed);
      sx1509.pwm(mot2PinA, 0);
      //handleRoot();
      delay(120);
  }


  void ForwardLeftSharp() {
      Serial.println("Robot: Received Go Left while moving forward");
        sx1509.pwm(mot1PinB, (Robot_Motor_Speed - 75));
        sx1509.pwm(mot1PinA, 0);


        sx1509.pwm(mot2PinB, Robot_Motor_Speed);
        sx1509.pwm(mot2PinA, 0);
        //handleRoot();
        delay(120);
    }



  void Right() {
    Serial.println("Robot: Received Go Right");
      sx1509.pwm(mot1PinA, 0);
      sx1509.pwm(mot1PinB, Robot_Motor_TurnSpeed);

      sx1509.pwm(mot2PinA, Robot_Motor_TurnSpeed);
      sx1509.pwm(mot2PinB, 0);

      //handleRoot();
      delay(140);
      /*if (Robot_Motor_Moving == true) {
        Forward();
      } else {
        Stop();
      }*/
  }


  void ForwardRight() {
      Serial.println("Robot: Received Go Right while moving forward");
      sx1509.pwm(mot1PinB, Robot_Motor_Speed);
      sx1509.pwm(mot1PinA, 0);


      sx1509.pwm(mot2PinB, (Robot_Motor_Speed - 20));
      sx1509.pwm(mot2PinA, 0);
      //handleRoot();
      delay(120);
  }



  void ForwardRightSharp() {
      Serial.println("Robot: Received Go Right while moving forward");
      sx1509.pwm(mot1PinB, Robot_Motor_Speed);
      sx1509.pwm(mot1PinA, 0);


      sx1509.pwm(mot2PinB, (Robot_Motor_Speed - 75));
      sx1509.pwm(mot2PinA, 0);
     // handleRoot();
      delay(120);
  }
// Left Right Code Parsing END

// Forward Reverse Code Parsing
  void Forward() {

    Serial.println("GoRobot: Received Go Forward");
    sx1509.pwm(mot1PinA, 0);
      sx1509.pwm(mot1PinB, Robot_Motor_Speed);        
      //analogWrite(mot1PinA, 0);
      //analogWrite(mot1PinB, mspeed);
      
      sx1509.pwm(mot2PinA, 0);
      sx1509.pwm(mot2PinB, Robot_Motor_Speed);   
      //analogWrite(mot2PinA, 0);
      //analogWrite(mot2PinB, mspeed);
      Robot_Motor_Moving = true;
      //handleRoot();
  }    

  void Reverse() {

    Serial.println("GoRobot: Received Go Forward");
    sx1509.pwm(mot1PinB, 0);
      sx1509.pwm(mot1PinA, Robot_Motor_Speed);        
      //analogWrite(mot1PinA, 0);
      //analogWrite(mot1PinB, mspeed);
      
      sx1509.pwm(mot2PinB, 0);
      sx1509.pwm(mot2PinA, Robot_Motor_Speed);   
      //analogWrite(mot2PinA, 0);
      //analogWrite(mot2PinB, mspeed);
      Robot_Motor_Moving = true;
      //handleRoot();
  }  
// Forward Reverse Code Parsing END

// Stop Code Parsing
  void Stop() {
    Serial.println("Robot: Received Stop Command");
    sx1509.pwm(mot1PinA, 0);
    sx1509.pwm(mot1PinB, 0);     

    sx1509.pwm(mot2PinA, 0);
    sx1509.pwm(mot2PinB, 0);     
    Robot_Motor_Moving = false;
    //handleRoot();
  }
// Stop Code Parsing END

// Print Wireless Information
  void printWifiData() {
    // print your WiFi shield's IP address:
    IPAddress ip = WiFi.localIP();
      Serial.print("IP Address: ");
    Serial.println(ip);
    Serial.println(ip);
    
    // print your MAC address:
    byte mac[6];  
    WiFi.macAddress(mac);
    Serial.print("MAC address: ");
    Serial.print(mac[5],HEX);
    Serial.print(":");
    Serial.print(mac[4],HEX);
    Serial.print(":");
    Serial.print(mac[3],HEX);
    Serial.print(":");
    Serial.print(mac[2],HEX);
    Serial.print(":");
    Serial.print(mac[1],HEX);
    Serial.print(":");
    Serial.println(mac[0],HEX);
   
  }
// Print Wireless Information END

// Print Current Network Status
  void printCurrentNet() {
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.println(rssi);
  }
// Print Current Network Status END
// Import Website
  /*bool loadFromFlash(String path) {
    if(path.endsWith("/")) path += "index.html";

    int NumFiles = sizeof(files)/sizeof(struct t_websitefiles);
    
    for(int i=0; i<NumFiles; i++) {
      if(path.endsWith(String(files[i].filename))) {      
        _FLASH_ARRAY<uint8_t>* filecontent;
        String dataType = "text/plain";
        unsigned int len = 0;
        
        dataType = files[i].mime;
        len = files[i].len;
        
        //server.sendHeader("Content-Length", String(len));
        
        
        filecontent = (_FLASH_ARRAY<uint8_t>*)files[i].content;
        
        filecontent->open();

        server.send(200, files[i].mime, *filecontent);


        WiFiClient client = server.client();
        client.write(*filecontent, 100);
        
        return true;
      }
    }
    
    return false;
  }*/

  bool loadFromFlash(String path) {
	  String dataType = "text/plain";
	  if(path.endsWith("/")) path += "index.html";

	  int NumFiles = sizeof(files)/sizeof(struct t_websitefiles);
	  
	  for(int i=0; i<NumFiles; i++) {
	    if(path.endsWith(String(files[i].filename))) {      
	      _FLASH_ARRAY<uint8_t>* filecontent;
	      
	      filecontent = (_FLASH_ARRAY<uint8_t>*)files[i].content;
	      server.streamFile(*filecontent, files[i].mime);
	      // server.send(200, files[i].mime, *filecontent);
	      return true;
	    }
	  }
	  
	  return false;
	}
// Import Website END



// WebServer Handler Not Found
  void handleNotFound() {
    
    // try to find the file in the flash
    if(loadFromFlash(server.uri())) return;
    
    String message = "File Not Found\n\n";
    message += "URI..........: ";
    message += server.uri();
    message += "\nMethod.....: ";
    message += (server.method() == HTTP_GET)?"GET":"POST";
    message += "\nArguments..: ";
    message += server.args();
    message += "\n";
    for (uint8_t i=0; i<server.args(); i++){
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    message += "\n";
    message += "FreeHeap.....: " + String(ESP.getFreeHeap()) + "\n";
    message += "ChipID.......: " + String(ESP.getChipId()) + "\n";
    message += "FlashChipId..: " + String(ESP.getFlashChipId()) + "\n";
    message += "FlashChipSize: " + String(ESP.getFlashChipSize()) + " bytes\n";
    message += "getCycleCount: " + String(ESP.getCycleCount()) + " Cycles\n";
    message += "Milliseconds.: " + String(millis()) + " Milliseconds\n";
    // server.send(404, "text/plain", message);
  }
// WebServer Handler Not Found END


