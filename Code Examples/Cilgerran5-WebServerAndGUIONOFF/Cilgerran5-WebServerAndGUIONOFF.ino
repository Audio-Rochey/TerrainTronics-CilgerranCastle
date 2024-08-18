/**
Dafydd Roche - 8/16/2024

This work is heavily based on the work by Rui Santos over at randomnerdtutorials.
Yes, there are better and smarter ways to make a webserver that does all this cool stuff. :)

First time you download this, the device will set up it's own wifi access point that you need to connect to.
Change the wifi on your PC to find it. It will be called: TerrainTronicsConfigMePlease

Connect to it, open your browser and go to http://192.168.4.1 - there you can configure it to connect to your home wifi.


*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com  
*********/


// Board specific stuff:
// The Cilgerran pins are defined as follows
#define inv_VLEDenable D0
#define D1Ledout D5
#define D2Ledout D6
#define D3Ledout D7
#define D4Ledout D8
#define D5Ledout D4
#define D6Ledout D3
#define MotA D1
#define MotB D2

// Double Reset Detect basics
#ifdef ESP8266
  #define ESP8266_DRD_USE_RTC     true
#endif
#include <ESP_DoubleResetDetector.h>      //https://github.com/khoih-prog/ESP_DoubleResetDetector
// Number of seconds after reset during which a 
// subseqent reset will be considered a double reset.
#define DRD_TIMEOUT 3
// RTC Memory Address for the DoubleResetDetector to use
#define DRD_ADDRESS 0
//DoubleResetDetector* drd;




#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String D1LedoutState = "off";
String D2LedoutState = "off";
String D3LedoutState = "off";
String D4LedoutState = "off";
String D5LedoutState = "off";
String D6LedoutState = "off";

// Assign output variables to GPIO pins
//const int output5 = 5;
//const int output4 = 4;

DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);

void setup() {
  Serial.begin(115200);  
   
  pinMode(D1Ledout, OUTPUT);
  pinMode(D2Ledout, OUTPUT);
  pinMode(D3Ledout, OUTPUT);
  pinMode(D4Ledout, OUTPUT);
  pinMode(D5Ledout, OUTPUT);
  pinMode(D6Ledout, OUTPUT);
  digitalWrite(D1Ledout, LOW);
  digitalWrite(D2Ledout, LOW);
  digitalWrite(D3Ledout, LOW);
  digitalWrite(D4Ledout, LOW);
  digitalWrite(D5Ledout, LOW);
  digitalWrite(D6Ledout, LOW);
    pinMode(inv_VLEDenable, OUTPUT);
  digitalWrite(inv_VLEDenable, HIGH); // Keep Cilgerran VLED Disabled

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  
if (drd.detectDoubleReset()) 
  {
    Serial.println("");
    Serial.println("*********************");
    Serial.println("Double Reset Detected");
    Serial.println("*********************");
    wifiManager.resetSettings();
  } 
  else 
  {
    Serial.println("No Double Reset Detected");
  }

  
  wifiManager.autoConnect("TerrainTronicsConfigMePlease");

  
  // if you get here you have connected to the WiFi
  Serial.println("Connected.");
  
  digitalWrite(inv_VLEDenable, LOW); // Enable Cilgerran VLED

  server.begin();


}

void loop(){
  
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /1/on") >= 0) {
              Serial.println("D1Ledout on");
              D1LedoutState = "on";
              digitalWrite(D1Ledout, HIGH);
            } else if (header.indexOf("GET /1/off") >= 0) {
              Serial.println("D1Ledout off");
              D1LedoutState = "off";
              digitalWrite(D1Ledout, LOW);
            } else if (header.indexOf("GET /2/on") >= 0) {
              Serial.println("D2Ledout on");
              D2LedoutState = "on";
              digitalWrite(D2Ledout, HIGH);
            } else if (header.indexOf("GET /2/off") >= 0) {
              Serial.println("D2Ledout off");
              D2LedoutState = "off";
              digitalWrite(D2Ledout, LOW);
            } else if (header.indexOf("GET /3/on") >= 0) {
              Serial.println("D3Ledout on");
              D3LedoutState = "on";
              digitalWrite(D3Ledout, HIGH);
            } else if (header.indexOf("GET /3/off") >= 0) {
              Serial.println("D3Ledout off");
              D3LedoutState = "off";
              digitalWrite(D3Ledout, LOW);
            } else if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("D4Ledout on");
              D4LedoutState = "on";
              digitalWrite(D4Ledout, HIGH);
            } else if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("D4Ledout off");
              D4LedoutState = "off";
              digitalWrite(D4Ledout, LOW);
            } else if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("D5Ledout on");
              D5LedoutState = "on";
              digitalWrite(D5Ledout, HIGH);
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("D5Ledout off");
              D5LedoutState = "off";
              digitalWrite(D5Ledout, LOW);
            } else if (header.indexOf("GET /6/on") >= 0) {
              Serial.println("D6Ledout on");
              D6LedoutState = "on";
              digitalWrite(D6Ledout, HIGH);
            } else if (header.indexOf("GET /6/off") >= 0) {
              Serial.println("D6Ledout off");
              D6LedoutState = "off";
              digitalWrite(D6Ledout, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>TerrainTronics Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for D1Ledout  
            client.println("<p>D1Ledout - State " + D1LedoutState + "</p>");
            // If the D1LedoutState is off, it displays the ON button       
            if (D1LedoutState=="off") {
              client.println("<p><a href=\"/1/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/1/off\"><button class=\"button D1Ledout\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for D2Ledout  
            client.println("<p>D2Ledout - State " + D2LedoutState + "</p>");
            // If the output4State is off, it displays the ON button       
            if (D2LedoutState=="off") {
              client.println("<p><a href=\"/2/on\"><button class=\"button \">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/2/off\"><button class=\"button D2Ledout\">OFF</button></a></p>");
            }

            // Display current state, and ON/OFF buttons for D3Ledout  
            client.println("<p>D3Ledout - State " + D3LedoutState + "</p>");
            // If the output4State is off, it displays the ON button       
            if (D3LedoutState=="off") {
              client.println("<p><a href=\"/3/on\"><button class=\"button \">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/3/off\"><button class=\"button D3Ledout\">OFF</button></a></p>");
            }

                        // Display current state, and ON/OFF buttons for D4Ledout  
            client.println("<p>D4Ledout - State " + D4LedoutState + "</p>");
            // If the output4State is off, it displays the ON button       
            if (D4LedoutState=="off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button \">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"button D4Ledout\">OFF</button></a></p>");
            }

                             // Display current state, and ON/OFF buttons for D5Ledout  
            client.println("<p>D5Ledout - State " + D5LedoutState + "</p>");
            // If the output4State is off, it displays the ON button       
            if (D5LedoutState=="off") {
              client.println("<p><a href=\"/5/on\"><button class=\"button \">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"button D5Ledout\">OFF</button></a></p>");
            }

                                         // Display current state, and ON/OFF buttons for D6Ledout  
            client.println("<p>D6Ledout - State " + D6LedoutState + "</p>");
            // If the output4State is off, it displays the ON button       
            if (D6LedoutState=="off") {
              client.println("<p><a href=\"/6/on\"><button class=\"button \">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/6/off\"><button class=\"button D6Ledout\">OFF</button></a></p>");
            }



            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  drd.loop();
}