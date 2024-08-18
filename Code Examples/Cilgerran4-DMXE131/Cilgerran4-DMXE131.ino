// WORKS WITH BOTH Q CONTROLLER LITE and PHOTON2.


//https://github.com/garymueller/ESP-131GPIO

/*
This code is based on Gary's above, mixed with some of Object Oriented programming stuff from Adafruit to describe LED behaviour.
So far, it mainly plays with the LED on the WemosD1Mini board.
It can either do direct output from Channel 5 in Q Light Controller, or it can copy a candle.

The code listens to 16 channels of data from Q Light Controller. (Universe and offset set by going to it's website)

To make this code work, you have to add the LittleFS files, and join the network then connect to 192.168.4.1

[Ctrl] + [Shift] + [P], then "Upload LittleFS to Pico/ESP8266/ESP32".

On macOS, press [⌘] + [Shift] + [P] to open the Command Palette in the Arduino IDE, then "Upload LittleFS to Pico/ESP8266/ESP32".
*/


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


#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESPAsyncE131.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncDNSServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <vector>

using namespace std;


// ***** USER SETUP STUFF *****
String ssid = "test"; // replace with your SSID.
String password = "testtest"; // replace with your password.

String CONFIG_FILE = "Config.json";
DynamicJsonDocument Config(1024);
AsyncWebServer server(80);
AsyncDNSServer dnsServer;
ESPAsyncE131 e131;


vector<int> GpioVector{16,5,4,0,2,14,12,13}; // only used by the relay page - ultimately needs removing.
int DigitalOn = HIGH;
int DigitalOff = LOW;

int noOfChannelsToListenTo = 16;
int DMXValue[16]; // 8 values are captured including 0.

//Forward Declarations
String WebReplace(const String& var);

bool LoadConfig();
void SaveConfig(AsyncWebServerRequest* request);

void InitGpio();
void InitWifi();
void Init131();
void InitWeb();
void E131Handler(); //Custom algo that reads the latest package and updates the Globals.



class Lightning // Flash uses the first DMX channel for brightness, the one next for speed. (range TBD)
{
  // Class Member Variables
  // These are initialized at startup
  int ledPin;      // the number of the LED pin
  int dmxchannel;     // Which data to listen to:

  // These maintain the current state
  int state;   
  int brightness;
  unsigned long previousMillis;   // will store last time LED was updated
  unsigned long delayTillNextState;        // delay between states

  // Constructor - creates a candle flicker
  // and initializes the member variables and state
  public:
  Lightning(int pin, int channel)
  {
  dmxchannel = channel;
  ledPin = pin;
  pinMode(ledPin, OUTPUT_OPEN_DRAIN);     
  state = 0;
  delayTillNextState = 0;  
  previousMillis = 0;
  }

  void Update()
  {
    // check to see if it's time to change the state of the LED
    unsigned long currentMillis = millis();
         
    if(currentMillis - previousMillis >= delayTillNextState)
    {
      brightness = DMXValue[dmxchannel];
      Serial.println(state);
      switch (state){
        case 0: // LEDCold: // Light off for a random time between 1 second and 4 seconds.
          analogWrite(ledPin, 255); // 
          pinMode(ledPin, OUTPUT_OPEN_DRAIN);
          state = 1;
          delayTillNextState = random(3000, 8000);
          //Serial.println(delayTillNextState);
          break;
          
        case 1: //LEDOnFlash: // Ballast Flash - Full power zap for 20mS (1/50th of a second)
          analogWrite(ledPin, (255-brightness)); // 
          pinMode(ledPin, OUTPUT_OPEN_DRAIN);
          state = 2;
          delayTillNextState = 30;
          break;
          
        case 2: //LEDoffFlash: // Ballast Flash Off - Zero for 1/50th of a second
          analogWrite(ledPin, 255); // 
          pinMode(ledPin, OUTPUT_OPEN_DRAIN);
          state = 0;
          delayTillNextState = 0;
          if (int(random(3)) == 2){ // will it repeat again?
            state = 1;
            delayTillNextState = 50;
          }
          break;
        }
      previousMillis = currentMillis;  // Remember the time
  }
  
  }
};

class Flasher // Flash uses the first DMX channel for brightness, the one next for speed. (range 100mS to 2Second) - Works with Cilgerran
{
  // Class Member Variables
  // These are initialized at startup
  int ledPin;      // the number of the LED pin
  int dmxchannel;     // Which data to listen to:

  // These maintain the current state
  bool newVal;                       // tracking on off.
  int brightness;
  long dmxonofftime;                 // tracks how long to flash for
  unsigned long previousMillis;   // will store last time LED was updated
  int data;

  // Constructor - creates a candle flicker
  // and initializes the member variables and state
  public:
  Flasher(int pin, int channel)
  {
  ledPin = pin;
  dmxchannel = channel;
  pinMode(ledPin, OUTPUT);     
  newVal = 0;
  previousMillis = 0;
  }

  void Update()
  {
    // check to see if it's time to change the state of the LED
    dmxonofftime = DMXValue[dmxchannel + 1];
    dmxonofftime = map(dmxonofftime, 1,255,100,2000);
    unsigned long currentMillis = millis();
    //data = DMXValue[dmxchannel]; 
    if(currentMillis - previousMillis >= dmxonofftime)
    {
      data = DMXValue[dmxchannel];
     
      newVal = !newVal;
      brightness = DMXValue[dmxchannel];
      analogWrite(ledPin, (newVal*brightness)); 

      previousMillis = currentMillis;  // Remember the time
    }
  }
};


class Candle // Cilgerran Working 8/14/24
{
  // Class Member Variables
  // These are initialized at startup
  int ledPin;      // the number of the LED pin
  int dmxchannel;     // Which data to listen to:

  // These maintain the current state
  int newVal;                       // New Value (to be randomly generated)
  int lowPassTrack;                 // lowpass tracker used to set the LED
  unsigned long previousMillis;   // will store last time LED was updated
  int data;

  // Constructor - creates a candle flicker
  // and initializes the member variables and state
  public:
  Candle(int pin, int channel)
  {
  ledPin = pin;
  dmxchannel = channel;
  pinMode(ledPin, OUTPUT);     
  newVal = 0;
  lowPassTrack = 0;  
  previousMillis = 0;
  }

  void Update()
  {
    // check to see if it's time to change the state of the LED
    unsigned long currentMillis = millis();
    //data = DMXValue[dmxchannel]; 
    if(currentMillis - previousMillis >= 100)
    {
      data = DMXValue[dmxchannel];
      newVal = (random(128) + (lowPassTrack/2));
      newVal = (int)map(newVal,0,255,0,data);
      analogWrite(ledPin, newVal); // 
      lowPassTrack = newVal;
      previousMillis = currentMillis;  // Remember the time
    
    }
  }
};

class DirectPWMOut // Working with Cilgerran 8/14/24
{
  // Class Member Variables
  // These are initialized at startup
  int ledPin;      // the number of the LED pin
  int dmxchannel;     // Which data to listen to:

  // These maintain the current state
  // not needed as the global is holding the current value
  int data;                 // ledState used to set the LED
  //unsigned long previousMillis;   // will store last time LED was updated

  // Constructor - 
  // and initializes the member variables and state
  public:
  DirectPWMOut(int pin, int channel)
  {
  
  

  ledPin = pin;
  dmxchannel = channel;
  pinMode(ledPin, OUTPUT);   
  //Serial.println("==============================");
  //Serial.printf("%d:%d; ",dmxchannel,data);
  //Serial.println("");
  //Serial.println("==============================");
  
  
  }

  void Update()
  {
    data = DMXValue[dmxchannel];
    analogWrite(ledPin, data);
    //pinMode(ledPin, OUTPUT);

  }
};

class DirectONOFF // 
{
  // Class Member Variables
  // These are initialized at startup
  int ledPin;      // the number of the LED pin
  int dmxchannel;     // Which data to listen to:
  int data;                 // ledState used to set the LED

  public:
  DirectONOFF(int pin, int channel)
  {
  ledPin = pin;
  dmxchannel = channel;
  pinMode(ledPin, OUTPUT);   

  
  }

  void Update()
  {
    data = DMXValue[dmxchannel];
    if (data > 127){
      digitalWrite(ledPin, HIGH);
    }
    else {
      digitalWrite(ledPin, LOW);
    }

  }
};




// DEFINE LED BEHAVIOUR HERE
// E.g. DirectPWMOut LEDA(D1Ledout, 0); //whhere D1Ledout is the output pins and "0" is the DMX channel
// Candle LEDA(D1Ledout, 0);
// Lightning is a special one. It uses 2 channels - so 


DirectPWMOut LEDA(D1Ledout, 0);
DirectPWMOut LEDB(D2Ledout, 1);
DirectPWMOut LEDC(D3Ledout, 2);
DirectPWMOut LEDD(D4Ledout, 3);
Candle LEDE(D5Ledout, 4);
DirectONOFF LEDF(D6Ledout, 5);




/*
 * Sets up the initial state and starts sockets
 */
void setup() {
  
  
  Serial.begin(115200);
  delay(500);
  Serial.println("==============================");
  Serial.println("Start Setup");
  
  LoadConfig();

  InitGpio();

  InitWifi();

  Init131();

  InitWeb();

  Serial.println("Completed setup. Starting loop");
  Serial.println("==============================");
  
  
  
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
  digitalWrite(inv_VLEDenable, LOW); // Enable Cilgerran VLED
}

/*
 * Main Event Loop
 */
void loop() {
  e131_packet_t packet;
  LEDA.Update();
  LEDB.Update();
  LEDC.Update();
  LEDD.Update();
  LEDE.Update();
  LEDF.Update();


  while(!e131.isEmpty()) {
    E131Handler();
  }
}// end void loop 




/*
 * E131Handler
 */
void E131Handler()
{
  e131_packet_t packet;
  bool Digital = Config["GPIO"]["digital"].as<bool>();
  int DigitalThreshold = Config["GPIO"]["digital_threshold"].as<int>();
  int ChannelOffset = Config["E131"]["channel_offset"].as<int>();
  e131.pull(&packet);     // Pull packet from ring buffer
    uint16_t num_channels = htons(packet.property_value_count) - 1;
    
    for(int GpioIndex = 0, ChannelIndex = ChannelOffset;
        GpioIndex < noOfChannelsToListenTo && ChannelIndex < num_channels; 
        ++GpioIndex, ++ChannelIndex) {
      
      uint16_t data = packet.property_values[ChannelIndex+1];
      DMXValue[GpioIndex] = data;

    }
}


/*
 * InitGpio
 */
void InitGpio()
{
  //initialize GPIO pins
  for(int i = 0; i < GpioVector.size(); ++i)  {
    pinMode(GpioVector[i], INPUT);
    //digitalWrite(GpioVector[i], DigitalOff);
  }
}

/*
 * Loads the config from SPPS into the Config variable
 */
bool LoadConfig()
{
  // Initialize LittleFS
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return false;
  }

  File file = LittleFS.open(CONFIG_FILE, "r");
  if (!file) {
    //First Run, setup defaults
    Config["network"]["hostname"] = "esps-" + String(ESP.getChipId(), HEX);
    Config["network"]["ssid"] = ssid;
    Config["network"]["password"] = password;
    Config["network"]["static"] = false;
    Config["network"]["static_ip"] = "192.168.1.100";
    Config["network"]["static_netmask"] = "255.255.255.0";
    Config["network"]["static_gateway"] = "192.168.1.1";
    Config["network"]["access_point"] = true;
    
    Config["E131"]["multicast"] = "true";
    Config["E131"]["universe"] = 1;
    Config["E131"]["channel_offset"] = 0;

    Config["GPIO"]["digital"] = "true";
    Config["GPIO"]["digital_threshold"] = 127;
    Config["GPIO"]["digital_lowlevel"] = false;
    
  } else {
    Serial.println("Loading Configuration File");
    deserializeJson(Config, file);
  }

  if(Config["GPIO"]["digital_lowlevel"].as<bool>()) {
    DigitalOn = LOW;
    DigitalOff = HIGH;
  } else {
    DigitalOn = HIGH;
    DigitalOff = LOW;
  }

  serializeJson(Config, Serial);Serial.println();
  return true;
}

void SaveConfig(AsyncWebServerRequest* request)
{
  Serial.println("Saving Configuration File");
  
  //debug
  int params = request->params();
  for(int i=0;i<params;i++){
    AsyncWebParameter* p = request->getParam(i);
    Serial.printf("Param: %s, %s\n", p->name().c_str(), p->value().c_str());
  }

  //Validate Config

  Config["network"]["hostname"] = request->getParam("hostname",true)->value();
  Config["network"]["ssid"] = request->getParam("ssid",true)->value();
  Config["network"]["password"] = request->getParam("password",true)->value();
  //checkbox status isnt always included if toggled off
  Config["network"]["static"] = 
    (request->hasParam("static",true) && (request->getParam("static",true)->value() == "on"));
  Config["network"]["static_ip"] = request->getParam("static_ip",true)->value();
  Config["network"]["static_netmask"] = request->getParam("static_netmask",true)->value();
  Config["network"]["static_gateway"] = request->getParam("static_gateway",true)->value();
  Config["network"]["access_point"] = 
    (request->hasParam("access_point",true) && (request->getParam("access_point",true)->value() == "on"));
  
  //checkbox status isnt always included if toggled off
  Config["E131"]["multicast"] = 
    (request->hasParam("multicast",true) && (request->getParam("multicast",true)->value() == "on"));
  Config["E131"]["universe"] = request->getParam("universe",true)->value();
  Config["E131"]["channel_offset"] = request->getParam("channel_offset",true)->value();

  //checkbox status isnt always included if toggled off
  Config["GPIO"]["digital"] =
    (request->hasParam("digital",true) && (request->getParam("digital",true)->value() == "on"));
  Config["GPIO"]["digital_threshold"] = request->getParam("digital_threshold",true)->value();
  Config["GPIO"]["digital_lowlevel"] =
    (request->hasParam("digital_lowlevel",true) && (request->getParam("digital_lowlevel",true)->value() == "on"));

  File file = LittleFS.open(CONFIG_FILE, "w");
  if(file) {
    serializeJson(Config, file);
    serializeJson(Config, Serial);
  }

  request->send(200);
  ESP.restart();
}

/*
 * Initialiaze Wifi (DHCP/STATIC and Access Point)
 */
void InitWifi()
{
  // Switch to station mode and disconnect just in case
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  WiFi.hostname(Config["network"]["hostname"].as<const char*>());

  //configure Static/DHCP
  if(Config["network"]["static"].as<bool>()) {
    IPAddress IP; IP.fromString(Config["network"]["static_ip"].as<String>());
    IPAddress Netmask; Netmask.fromString(Config["network"]["static_netmask"].as<String>());
    IPAddress Gateway; Gateway.fromString(Config["network"]["static_gateway"].as<String>());

    if(WiFi.config(IP, Netmask, Gateway)) {
      Serial.println("Successfully configured static IP");
    } else {
      Serial.println("Failed to configure static IP");
    }
  } else {
    Serial.println("Connecting with DHCP");
  }

  //Connect
  int Timeout = 15000;
  WiFi.begin(Config["network"]["ssid"].as<String>(), Config["network"]["password"].as<String>());
  if(WiFi.waitForConnectResult(Timeout) != WL_CONNECTED) {
    if(Config["network"]["access_point"].as<bool>()) {
      Serial.println("*** FAILED TO ASSOCIATE WITH AP, GOING SOFTAP ***");
      WiFi.mode(WIFI_AP);
      WiFi.softAP(Config["network"]["hostname"].as<String>());
      dnsServer.start(53, "*", WiFi.softAPIP());
    } else {
      Serial.println(F("*** FAILED TO ASSOCIATE WITH AP, REBOOTING ***"));
      ESP.restart();
    }
  } else {
    Serial.printf("Connected as %s\n",WiFi.localIP().toString().c_str());
  }

  WiFi.printDiag(Serial);
}

void Init131()
{
  if(Config["E131"]["multicast"].as<bool>()) {
    Serial.printf("Initializing Multicast with universe <%d>\n",Config["E131"]["universe"].as<int>());
    e131.begin(E131_MULTICAST, Config["E131"]["universe"]);
  } else {
    Serial.println("Initializing Unicast");
    e131.begin(E131_UNICAST);
  }
}


/*
 * Initializes the webserver
 */
void InitWeb()
{
  //enables redirect to /index.html on AP connection
  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", String(), false, WebReplace);
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", String(), false, WebReplace);
  });
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/favicon.png", "image/png");
  });
  server.on("/SaveConfig", HTTP_POST, [](AsyncWebServerRequest *request){
    SaveConfig(request);
  });
  server.on("/SetRelay", HTTP_GET, [](AsyncWebServerRequest *request){
    int relay = request->getParam("relay")->value().toInt();
    if(relay<0 || relay >= GpioVector.size()) {
      Serial.println("SetRelay - Index out of range");
      return;
    }
    digitalWrite(GpioVector[relay], (request->getParam("checked")->value() == "true") ? DigitalOn : DigitalOff);
    request->send(200);
  });

  server.begin();
}

/*
 * WebReplace:
 * Substitutes variables inside of the html
 */
String WebReplace(const String& var)
{
  //Status Page
  if (var == "SSID") {
    return (String)WiFi.SSID();
  } else if (var == "HOSTNAME") {
    return (String)WiFi.hostname();
  } else if (var == "IP") {  
    if(WiFi.getMode() == WIFI_AP) {
      return WiFi.softAPIP().toString();
    } else {
      return WiFi.localIP().toString();
    }
  } else if (var == "MAC") {
    return (String)WiFi.macAddress();
  } else if (var == "RSSI") {
    return (String)WiFi.RSSI();
  } else if (var == "HEAP") {
    return (String)ESP.getFreeHeap();
  } else if (var == "UPTIME") {
    return String(millis());
  } else if (var == "UNIVERSE") {
    return Config["E131"]["universe"];
  } else if (var == "PACKETS") {
    return (String)e131.stats.num_packets;
  } else if (var == "PACKET_ERR") {
    return (String)e131.stats.packet_errors;
  } else if (var == "LAST_IP") {
    return e131.stats.last_clientIP.toString();

  //Configuration Page   
  } else if (var == "CONFIG_HOSTNAME") {
    return Config["network"]["hostname"];
  } else if (var == "CONFIG_SSID") {
    return Config["network"]["ssid"];
  } else if (var == "CONFIG_PASSWORD") {
    return Config["network"]["password"];
  } else if (var == "CONFIG_AP") {
    if(Config["network"]["access_point"].as<bool>())
      return "checked";
     else
      return "";
  } else if (var == "CONFIG_STATIC") {
    if(Config["network"]["static"].as<bool>())
      return "checked";
     else
      return "";
  } else if (var == "CONFIG_STATIC_IP") {
    return Config["network"]["static_ip"];
  } else if (var == "CONFIG_STATIC_NETMASK") {
    return Config["network"]["static_netmask"];
  } else if (var == "CONFIG_STATIC_GATEWAY") {
    return Config["network"]["static_gateway"];
  } else if (var == "CONFIG_MULTICAST") {
    if(Config["E131"]["multicast"].as<bool>())
      return "checked";
     else
      return "";
  } else if (var == "CONFIG_UNIVERSE") {
    return Config["E131"]["universe"];
  } else if (var == "CONFIG_CHANNEL_OFFSET") {
    return Config["E131"]["channel_offset"];
  } else if (var == "CONFIG_DIGITAL") {
    if(Config["GPIO"]["digital"].as<bool>())
      return "checked";
     else
      return "";
  } else if (var == "CONFIG_THRESHOLD") {
    return Config["GPIO"]["digital_threshold"];
  } else if (var == "CONFIG_LOWLEVEL") {
    if(Config["GPIO"]["digital_lowlevel"].as<bool>())
      return "checked";
     else
      return "";

  //Relay Page
  } else if (var == "RELAYS") {
    String Relays = "";
    for(int i = 0; i < GpioVector.size(); ++i) {
      Relays += "<label>Relay "+String(i+1)+" ("+GpioVector[i]+")</label>";
      Relays += "  <label class=\"switch\">";
      Relays += "  <input type=\"checkbox\" ";
      if(digitalRead(GpioVector[i]) == DigitalOn) {
        Relays += "checked";
      }
      Relays += " onclick=\"fetch('SetRelay?relay="+String(i)+"&checked='+this.checked);\">";
      Relays += "  <span class=\"slider round\"></span>";
      Relays += "</label>";
      Relays += "<br><br>";
    }
    return Relays;
  }
 
  return var;
}
