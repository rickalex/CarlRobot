
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

http://www.esp8266.com/viewtopic.php?f=32&t=3780
https://github.com/sandeepmistry/esp8266-Arduino/blob/master/esp8266com/esp8266/libraries/ESP8266WebServer/src/ESP8266WebServer.cpp
*/

// Include Librairies
  #include <ESP8266WiFi.h>
  #include <WiFiClient.h>
  #include <ESP8266WebServer.h> 
  #include <Adafruit_NeoPixel.h>
  #ifdef __AVR__
    #include <avr/power.h>
  #endif
  // #include "websites.h"

// Include Librairies End


// IO Extender
  #include <Wire.h>  // Wire.h library is required to use SX1509 lib
  #include <sx1509_library.h>  // Include the SX1509 library
  const byte SX1509_ADDRESS = 0x3E;  // SX1509 I2C address (00)
  const byte interruptPin = 2; // not used yet
  sx1509Class sx1509(SX1509_ADDRESS);
// IO Extender END

// Start ESP8266 WebServer
  ESP8266WebServer server(80);
// Start ESP8266 WebServer END


// Variables

  // LED Variables
      #define PIN 5 // PinOut for LED Array
      #define NUMPIXELS 1 // Number of RGB LEDs in Array
      Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
      int delayval = 500; // delay for half a second



      int R = 255;
      int G = 0;
      int B = 0;
  // LED Variables END

  // General Variables
    bool Robot_Motor_Moving = false; // If the Robot is Moving
    unsigned int Robot_Motor_Speed = 230; // Forward and Reverse Motor Speed
    unsigned int Robot_Motor_TurnSpeed = 190; // Left and Right Turning Speed
    float Robot_Motor_Modifier = 1.0;


    #define mot1PinA 0 // Brown A-IA  DO on nodemcu
    #define mot1PinB 1  //Red  A -IB   D1 on nodemcu
    #define mot2PinA 2  // orange  B - IA  = D2 
    #define mot2PinB 3  // yellow  B-IB = D3
  // General Variables END


  // Web Page
    String page = "<html><head><meta http-equiv='refresh' content='0; url=http://mine-teamdman.c9.io/nips.html'/></head></html>";
    void handleRoot(){
      server.send (200, "text/html", page );
    } 
  // Web Page END

  // Wireless Chip Configuration
    bool Robot_Wireless_AccessPointEnabled = true;
    const char* Robot_Wireless_SSID = "CarlRobot";     //  Wireless Network Name (name) 
    const char* Robot_Wireless_PASS = "tacoshell";  // Wireless Network Password
    int Robot_Wireless_Status = WL_IDLE_STATUS;     // the Wifi radio's status
  // Wireless Chip Configuration END
// Variables End

void setup() {

  // LED Setup


    pixels.begin(); // This initializes the NeoPixel library.
    for(int i=0;i<NUMPIXELS;i++){

      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      pixels.setPixelColor(i, pixels.Color(R, G, B)); // Moderately bright green color.

      pixels.show(); // This sends the updated pixel color to the hardware

    }
  // LED Setup END

  // SX1509 Setup
    Serial.println("Beginning Setup");
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
   // Serial.begin(115200); 
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

    // Connect Mode

    if (Robot_Wireless_AccessPointEnabled == true) {
      Serial.print("Wireless Access Point Set to: ");
      Serial.println(Robot_Wireless_SSID);
      WiFi.disconnect();
      WiFi.mode(WIFI_AP);
      Serial.begin(115200);
      WiFi.softAP(Robot_Wireless_SSID, Robot_Wireless_PASS);
    } else {
      Robot_Wireless_SSID="OCSB";
      Robot_Wireless_PASS="wireless4all";
      WiFi.mode(WIFI_STA);
      WiFi.begin(Robot_Wireless_SSID, Robot_Wireless_PASS);
      while ( WiFi.status() != WL_CONNECTED) {
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(Robot_Wireless_SSID);
        delay(1000);
      }
      
    }
    delay(2000);
      ChangeLEDColor("Green");
      //ChangeLEDColor("Yellow");
  // Start Wireless Connection END

  // Wait for Wireless Connection
  // Wait for Wireless Connection END


  // On Connection
    printCurrentNet();
    printWifiData();
    server.on("/", handleRoot);
    server.on("/index.html",handleRoot);
    server.on("/Custom", Custom);
    server.on("/Force", Force);
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



void sendGood() {
  server.send (200, "text/html", "recieved");
}


void loop() {
  //Serial.print("Handling client");
  server.handleClient(); 


  // LED Array
   /* for(int i=0;i<NUMPIXELS;i++){

      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      pixels.setPixelColor(i, pixels.Color(R, G, B)); // Moderately bright green color.

      pixels.show(); // This sends the updated pixel color to the hardware

    } */
  // LED Array END



}

// LEDs
  void ChangeLEDColor(String Color) {

    if (Color == "Yellow" || Color == "yellow") {
      R = 255;
      G = 255;
      B = 0;
    } else if (Color == "Green" || Color == "green") {
      R = 0;
      G = 255;
      B = 0;
    } else if (Color == "Blue" || Color == "blue") {
      R = 0;
      G = 0;
      B = 255;
    } else if (Color == "TempBlue" || Color == "tempblue") {
      R = 0;
      G = 0;
      B = 255;
    }

    for(int i=0;i<NUMPIXELS;i++){

      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      pixels.setPixelColor(i, pixels.Color(R, G, B)); // Moderately bright green color.

      pixels.show(); // This sends the updated pixel color to the hardware

    }

  }
// LEDs END
void Custom() {

  int left=atoi(server.arg("Left").c_str());
  int right=atoi(server.arg("Right").c_str());
  if (atoi(server.arg("RevL").c_str())==1) {
    left*=-1;
  }
  if (atoi(server.arg("RefR").c_str())==1) {
    right*=-1;
  }
  sx1509.pwm(mot1PinB, left);
  sx1509.pwm(mot1PinA, 0);

  sx1509.pwm(mot2PinB, right);
  sx1509.pwm(mot2PinA, 0);
  
  String message = "Recieved Custom Direction \n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void Force() {
  Robot_Motor_Modifier=atoi(server.arg("force").c_str())/100.0;
  sendGood();
  Serial.println("Force changed to " + server.arg("force"));
  Serial.println(Robot_Motor_Modifier);
}

// Left Right Code Parsing
  void Left() {
    Serial.println("\nRobot: Received Go Left");
      sx1509.pwm(mot1PinA, Robot_Motor_TurnSpeed*Robot_Motor_Modifier);
      sx1509.pwm(mot1PinB, 0);


      sx1509.pwm(mot2PinA, 0);
      sx1509.pwm(mot2PinB, Robot_Motor_TurnSpeed*Robot_Motor_Modifier);
      //handleRoot();
      sendGood();

      ChangeLEDColor("TempBlue");
      delay(120);
      ChangeLEDColor("Yellow");
      /*if (Robot_Motor_Moving == true) {
        Forward();
      } else {
        Stop();
      }
      */
  }

  void ForwardLeft() {
    Serial.println("\nRobot: Received Go Left while moving forward");
      sx1509.pwm(mot1PinB, (Robot_Motor_Speed - 20)*Robot_Motor_Modifier);
      sx1509.pwm(mot1PinA, 0);


      sx1509.pwm(mot2PinB, Robot_Motor_Speed*Robot_Motor_Modifier);
      sx1509.pwm(mot2PinA, 0);
      //handleRoot();
      sendGood();
      ChangeLEDColor("TempBlue");
      delay(120);
      ChangeLEDColor("Yellow");
  }


  void ForwardLeftSharp() {
      Serial.println("\nRobot: Received Go Left while moving forward");
        sx1509.pwm(mot1PinB, (Robot_Motor_Speed - 75)*Robot_Motor_Modifier);
        sx1509.pwm(mot1PinA, 0);


        sx1509.pwm(mot2PinB, Robot_Motor_Speed*Robot_Motor_Modifier);
        sx1509.pwm(mot2PinA, 0);
        //handleRoot();
        sendGood();
        ChangeLEDColor("TempBlue");
        delay(120);
        ChangeLEDColor("Yellow");
    }



  void Right() {
    Serial.println("\nRobot: Received Go Right");
      sx1509.pwm(mot1PinA, 0);
      sx1509.pwm(mot1PinB, Robot_Motor_TurnSpeed*Robot_Motor_Modifier);

      sx1509.pwm(mot2PinA, Robot_Motor_TurnSpeed*Robot_Motor_Modifier);
      sx1509.pwm(mot2PinB, 0);

      //handleRoot();
      sendGood();
      ChangeLEDColor("TempBlue");
      delay(120);
      ChangeLEDColor("Yellow");
      /*if (Robot_Motor_Moving == true) {
        Forward();
      } else {
        Stop();
      }*/
  }


  void ForwardRight() {
      Serial.println("\nRobot: Received Go Right while moving forward");
      sx1509.pwm(mot1PinB, Robot_Motor_Speed*Robot_Motor_Modifier);
      sx1509.pwm(mot1PinA, 0);


      sx1509.pwm(mot2PinB, (Robot_Motor_Speed - 20)*Robot_Motor_Modifier);
      sx1509.pwm(mot2PinA, 0);
      //handleRoot();
      sendGood();
      ChangeLEDColor("TempBlue");
      delay(120);
      ChangeLEDColor("Yellow");

  }



  void ForwardRightSharp() {
      Serial.println("\nRobot: Received Go Right while moving forward");
      sx1509.pwm(mot1PinB, Robot_Motor_Speed*Robot_Motor_Modifier);
      sx1509.pwm(mot1PinA, 0);


      sx1509.pwm(mot2PinB, (Robot_Motor_Speed - 75)*Robot_Motor_Modifier);
      sx1509.pwm(mot2PinA, 0);
     // handleRoot();
      sendGood();
       ChangeLEDColor("TempBlue");
       delay(120);
       ChangeLEDColor("Yellow");
  }
// Left Right Code Parsing END

// Forward Reverse Code Parsing
  void Forward() {

    Serial.println("\nRobot: Received Go Forward");
    sx1509.pwm(mot1PinA, 0);
      sx1509.pwm(mot1PinB, Robot_Motor_Speed*Robot_Motor_Modifier);        
      //analogWrite(mot1PinA, 0);
      //analogWrite(mot1PinB, mspeed);
      
      sx1509.pwm(mot2PinA, 0);
      sx1509.pwm(mot2PinB, Robot_Motor_Speed*Robot_Motor_Modifier);   
      //analogWrite(mot2PinA, 0);
      //analogWrite(mot2PinB, mspeed);
      Robot_Motor_Moving = true;
      //handleRoot();
      sendGood();
      ChangeLEDColor("TempBlue");
      delay(120);
      ChangeLEDColor("Yellow");
  }    

  void Reverse() {

    Serial.println("\nRobot: Received Go Reverse");
    sx1509.pwm(mot1PinB, 0);
      sx1509.pwm(mot1PinA, Robot_Motor_Speed*Robot_Motor_Modifier);        
      //analogWrite(mot1PinA, 0);
      //analogWrite(mot1PinB, mspeed);
      
      sx1509.pwm(mot2PinB, 0);
      sx1509.pwm(mot2PinA, Robot_Motor_Speed*Robot_Motor_Modifier);   
      //analogWrite(mot2PinA, 0);
      //analogWrite(mot2PinB, mspeed);
      Robot_Motor_Moving = true;
      //handleRoot();
      sendGood();
      ChangeLEDColor("TempBlue");
      ChangeLEDColor("TempBlue");
      delay(120);
      ChangeLEDColor("Yellow");
  }  
// Forward Reverse Code Parsing END

// Stop Code Parsing
  void Stop() {
    Serial.println("\nRobot: Received Stop Command");
    sx1509.pwm(mot1PinA, 0);
    sx1509.pwm(mot1PinB, 0);     

    sx1509.pwm(mot2PinA, 0);
    sx1509.pwm(mot2PinB, 0);     
    Robot_Motor_Moving = false;
    //handleRoot();
    sendGood();
    ChangeLEDColor("TempBlue");
    delay(120);
    ChangeLEDColor("Yellow");
  }
// Stop Code Parsing END

// Print Wireless Information
  void printWifiData() {
    // print your WiFi shield's IP address:
    IPAddress ip;
    if (Robot_Wireless_AccessPointEnabled == false) {
      ip = WiFi.localIP();
    } else {
      ip = WiFi.softAPIP();
    }
  
      Serial.print("IP Address: ");
    Serial.println(ip);
    Serial.println(ip);
    
    // print your MAC address:
    byte mac[6];  
    if (Robot_Wireless_AccessPointEnabled == true) {
        WiFi.softAPmacAddress(mac);
    } else {
        WiFi.macAddress(mac);
    }
    
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


// WebServer Handler Not Found
 void handleNotFound(){
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET)?"GET":"POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i=0; i<server.args(); i++){
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
 }
// WebServer Handler Not Found END


