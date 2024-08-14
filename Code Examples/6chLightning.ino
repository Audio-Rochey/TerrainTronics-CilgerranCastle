/*

Wemos D1 code to work with the Cilgerran Castle
Dafydd Roche - 7/11/24
This code is open source.

This "Object Oriented Programming" model for the LED's is is based heavily around the work done by adafruit on: https://learn.adafruit.com/multi-tasking-the-arduino-part-1

I created six different classes of LED patterns.
- Flasher (based on Adafruits) - set it with a Pin, Time on and Time off.
- Breathe - Set it with a Pin, and the time for it to rise and fall and a max value.
- GauessBreathe - A different algorithm - takes 3 variables
- Candle - just pick a pin. Borrowed and hacked from: https://github.com/cpldcpu/SimPad/blob/master/Toolchain/examples/candleflicker/candleflicker.c
- Neon - Just Pick a pin.
- Lightning - Pick a pin.

****** INSTRUCTIONS *******
Look for the section below that reads "THIS IS WHERE YOU CONFIGURE YOUR OUTPUTS /"
Use the examples and use D1Ledout through D6Ledout.
If your USB Power Bank keeps powering off, set one of the LED's to be "Flasher keepalive(D*Ledout, 1000, 20000); // replace D*Ledout with the output you want to use"
Then add a 47Ohm resistor between the pins (look in the manual.) That will push enough current for 1 second, every 20 seconds, to trick the power supply into staying on.



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

#include <ESP8266WiFi.h>

class Flasher // WORKING 7/11/24
{
  // Class Member Variables
  // These are initialized at startup
  int ledPin;      // the number of the LED pin
  long OnTime;     // milliseconds of on-time
  long OffTime;    // milliseconds of off-time

  // These maintain the current state
  int ledState;                 // ledState used to set the LED
  unsigned long previousMillis;   // will store last time LED was updated

  // Constructor - creates a Flasher 
  // and initializes the member variables and state
  public:
  Flasher(int pin, long on, long off)
  {
  ledPin = pin;
  pinMode(ledPin, OUTPUT);     
    
  OnTime = on;
  OffTime = off;
  
  ledState = LOW; 
  previousMillis = 0;
  }

  void Update()
  {
    // check to see if it's time to change the state of the LED
    unsigned long currentMillis = millis();
     
    if((ledState == HIGH) && (currentMillis - previousMillis >= OnTime))
    {
      ledState = LOW;  // Turn it off
      previousMillis = currentMillis;  // Remember the time
      digitalWrite(ledPin, ledState);  // Update the actual LED
    }
    else if ((ledState == LOW) && (currentMillis - previousMillis >= OffTime))
    {
      ledState = HIGH;  // turn it on
      previousMillis = currentMillis;   // Remember the time
      digitalWrite(ledPin, ledState);   // Update the actual LED
    }
  }
};

class Breathe //WORKING 7/11/24
{
  // Class Member Variables
  // These are initialized at startup
  int ledPin;      // the number of the LED pin
  long Rate;     // milliseconds of up and down time
  //long OnTime;     // milliseconds of on-time
  //long OffTime;    // milliseconds of off-time

  // These maintain the current state
  int CurrentValue;                 // Used to track the current Value of the LED
  int Direction;                     // Used to track the current direction
  unsigned long previousMillis;   // will store last time LED was updated
  unsigned long updateMillisRate;  // Stores a value that "time between steps"

  // Constructor - creates a Flasher 
  // and initializes the member variables and state
  public:
  Breathe(int pin, long Rate)
  {
  ledPin = pin;
  pinMode(ledPin, OUTPUT);     
  previousMillis = 0;
  CurrentValue = 0;
  Direction = 1;
  updateMillisRate = Rate / 510; // This calculated how many milliseconds per change. 510= 255 steps *2 
  }

  void Update()
  {
    // check to see if it's time to change the state of the LED
    unsigned long currentMillis = millis();
     
    if(currentMillis - previousMillis >= updateMillisRate)
    {
      CurrentValue = CurrentValue + Direction;
      analogWrite(ledPin, CurrentValue);
      pinMode(ledPin, OUTPUT); 
      if (CurrentValue == 255){ 
        Direction = -1;
        }
      else if (CurrentValue == 0) {
        Direction = 1;
      }
      previousMillis = currentMillis;  // Remember the time
    }
  }
};

class GaussBreathe // WORKING 7/11/24
{
  // Class Member Variables
  // These are initialized at startup
  int ledPin;      // the number of the LED pin
  long Rate;     // milliseconds of up and down time


  // These maintain the current state
  int CurrentValue;                 // Used to track the current Value of the LED
  int Direction;                     // Used to track the current direction
  unsigned long previousMillis;   // will store last time LED was updated
  unsigned long updateMillisRate;  // Stores a value that "time between steps"
  float smoothness_pts;//larger=slower change in brightness  
  float gamma; // affects the width of peak (more or less darkness)
  float beta;; // shifts the gaussian to be symmetric*/
  float ii = 0;

  // Constructor - creates a Flasher 
  // and initializes the member variables and state
  public:
  GaussBreathe(int pin,float inputsmoothness_pts, float inputgamma)
  {
  smoothness_pts = inputsmoothness_pts;//larger=slower change in brightness  
  gamma = inputgamma; // affects the width of peak (more or less darkness)
  beta = 0.5; // shifts the gaussian to be symmetric
  ledPin = pin;
  pinMode(ledPin, OUTPUT);     
  previousMillis = 0;
  CurrentValue = 0;
  Direction = 1;
  }

  void Update()
  {
    // check to see if it's time to change the state of the LED
    unsigned long currentMillis = millis();
     
    if(currentMillis - previousMillis >= 5) // update every 5 miliseconds
    {
      float pwm_val = 255.0*(exp(-(pow(((ii/smoothness_pts)-beta)/gamma,2.0))/2.0)); // borrowed from https://makersportal.com/blog/2020/3/27/simple-breathing-led-in-arduino
      analogWrite(ledPin, (255-int(pwm_val))); // subtracted from 255 because we're open collector.
      pinMode(ledPin, OUTPUT); 
      ii++;
      if (ii == smoothness_pts){
        ii = 0;
      }
      previousMillis = currentMillis;  // Remember the time
    }
  }
};

class Candle // WORKING 7/11/24
{
  // Class Member Variables
  // These are initialized at startup
  int ledPin;      // the number of the LED pin

  // These maintain the current state
  int newVal;                       // New Value (to be randomly generated)
  int lowPassTrack;                 // lowpass tracker used to set the LED
  unsigned long previousMillis;   // will store last time LED was updated

  // Constructor - creates a candle flicker
  // and initializes the member variables and state
  public:
  Candle(int pin)
  {
  ledPin = pin;
  pinMode(ledPin, OUTPUT);     
  newVal = 0;
  lowPassTrack = 0;  
  previousMillis = 0;
  }

  void Update()
  {
    // check to see if it's time to change the state of the LED
    unsigned long currentMillis = millis();
     
    if(currentMillis - previousMillis >= 100)
    {
      newVal = random(128) + (lowPassTrack/2);
      analogWrite(ledPin, newVal); // subtracted from 255.
      pinMode(ledPin, OUTPUT); 
      lowPassTrack = newVal;
      previousMillis = currentMillis;  // Remember the time
    }
  }
};

class Neon // WORKING 7/11/24
{
  // Class Member Variables
  // These are initialized at startup
  int ledPin;      // the number of the LED pin

  // These maintain the current state
  int state;                      // Define the state (0 = off, 1 = Pre-Flash, 2 Flash On, 3 Flash Off)
  unsigned long delayTillNextState;        // delay between states
  unsigned long previousMillis;   // will store last time LED was updated

  // Constructor - creates a candle flicker
  // and initializes the member variables and state
  public:
  Neon(int pin)
  {
  ledPin = pin;
  pinMode(ledPin, OUTPUT);     
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
      switch (state){
        case 0: // LEDCold: // Light off for a random time between 1 second and 4 seconds.
          analogWrite(ledPin, 255); // 
          pinMode(ledPin, OUTPUT);
          state = 1;
          delayTillNextState = random(500, 1500);
          break;
          
        case 1: //LEDWarmup: // Ballast warming - Light on a little bit for 0.5 to 1.5 seocnds
          analogWrite(ledPin, 253); // 
          pinMode(ledPin, OUTPUT);
          state = 2;
          delayTillNextState = random(150, 650);
          break;
          
        case 2: //LEDOnFlash: // Ballast Flash - Full power zap for 20mS (1/50th of a second)
          analogWrite(ledPin, 0); // 
          pinMode(ledPin, OUTPUT);
          state = 3;
          delayTillNextState = 70;
          break;
          
        case 3: //LEDoffFlash: // Ballast Flash Off - Zero for 1/50th of a second
          analogWrite(ledPin, 255); // 
          pinMode(ledPin, OUTPUT);
          state = 0;
          delayTillNextState = random(500, 1000);
          if (int(random(3)) == 2){ // will it repeat again?
            state = 2;
            delayTillNextState = 50;
          }
          break;
        }
      previousMillis = currentMillis;  // Remember the time
  }
  
  }
};

class Lightning
{
  // Class Member Variables
  // These are initialized at startup
  int ledPin;      // the number of the LED pin

  // These maintain the current state
  int state;                      // Define the state (0 = off, 1 = Pre-Flash, 2 Flash On, 3 Flash Off)
  unsigned long delayTillNextState;        // delay between states
  unsigned long previousMillis;   // will store last time LED was updated

  // Constructor - creates a candle flicker
  // and initializes the member variables and state
  public:
  Lightning(int pin)
  {
  ledPin = pin;
  pinMode(ledPin, OUTPUT);     
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
      switch (state){
        case 0: // LEDCold: // Light off for a random time between 1 second and 4 seconds.
          analogWrite(ledPin, 0); // 
          pinMode(ledPin, OUTPUT);
          state = 1;
          delayTillNextState = random(3000, 8000);
          delayTillNextState = delayTillNextState + (random(0,1000)); // mainly dealing with startup delay.
          break;
          
        case 1: //LEDOnFlash: // Ballast Flash - Full power zap for 20mS (1/50th of a second)
          analogWrite(ledPin, 255); // 
          pinMode(ledPin, OUTPUT);
          state = 2;
          delayTillNextState = 30;
          break;
          
        case 2: //LEDoffFlash: // Ballast Flash Off - Zero for 1/50th of a second
          analogWrite(ledPin, 0); // 
          pinMode(ledPin, OUTPUT);
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

class FadeInFadeOut
{
  // Class Member Variables
  // These are initialized at startup
  int ledPin;      // the number of the LED pin
  unsigned long fadeInStepTime; // time between updating brightness on fade in
  unsigned long fadeOutStepTime; // same, but fade out.

  // These maintain the current state
  int state;                      // Define the state (0 = off, 1 = FadeIn, 2 Flash On, 3 Flash Off)
  unsigned long delayTillNextState;        // delay between states
  unsigned long previousMillis;   // will store last time LED was updated
  int ledBrightness;

  // Constructor - creates a candle flicker
  // and initializes the member variables and state
  public:
  FadeInFadeOut(int pin, unsigned long INfadeInStepTime, unsigned long INfadeOutStepTime)
  {
  ledPin = pin;
  pinMode(ledPin, OUTPUT);
  state = 0;
  delayTillNextState = 0;  
  previousMillis = 0;
  fadeInStepTime = INfadeInStepTime;
  fadeOutStepTime = INfadeOutStepTime;
  }

  void switchPress(){
    state++;
    if (state==4){
            state = 0;
          }
    Serial.println(state);
  }

  void Update()
  {
    // check to see if it's time to change the state of the LED
    unsigned long currentMillis = millis();
         
    if(currentMillis - previousMillis >= delayTillNextState)
    {
      switch (state){
        case 0: // LED Off.
          ledBrightness = 255;
          analogWrite(ledPin, ledBrightness); // 
          pinMode(ledPin, OUTPUT);
          delayTillNextState = 5000;
          //state = 1; // for debug
          break;
          
        case 1: // Fade In
          analogWrite(ledPin, ledBrightness--); // 
          pinMode(ledPin, OUTPUT);
          if (ledBrightness==0){
            state = 2;
          }
          delayTillNextState = fadeInStepTime;
          break;
          
        case 2: //  All On and stay on until further notice.
          analogWrite(ledPin, 0); // 
          pinMode(ledPin, OUTPUT);
          delayTillNextState = 100;
          //state = 3;
          break;

        case 3: // Fade Out
          analogWrite(ledPin, ledBrightness++); // 
          pinMode(ledPin, OUTPUT);
          if (ledBrightness==255){
            state = 0;
          }
          delayTillNextState = fadeOutStepTime;
          break;

          
        }
      previousMillis = currentMillis;  // Remember the time
  }
  
  }
};


/* THIS IS WHERE YOU CONFIGURE YOUR OUTPUTS */

//Examples:
//Flasher ledA(D2, 1000, 1000); // which pin, time on, time off in miliseconds (1000 per second)
//Neon ledA(D1);//easy - set the pin and that's it
//GaussBreathe ledB(D2, 2000, 0.14); // which (pin, smoothnesspoints, gamma) - smoothness points - more points makes the cycle take longer, gamma is ratio of dark to bright. suggest 0.14
//Candle led2(D2); // just call the pin you want for candle effects.
//Lightning ledA(D1);
//FadeInFadeOut(int pin, unsigned long fadeInStepTime, unsigned long fadeOutStepTime)

Lightning ledA(D1Ledout);
Lightning ledB(D2Ledout);
Lightning ledC(D3Ledout);
Lightning ledD(D4Ledout);
Lightning ledE(D5Ledout);
Lightning ledF(D6Ledout);




void setup()
{
  Serial.begin(9600);
  
  //These commands disable the Wifi to save power. Clearly, these need to be removed if you're doing to do cool DMX-ARtnet stuff.
  // Doing this drops the idle power consumption to 14mA, with fired LED's pulling a few hundred mA for 20mS at a time.
  // In other words, 1000mAH battery will last between 48 and 72 hours of continuous output.
  WiFiMode(WIFI_STA);
  WiFi.disconnect(); 
  WiFi.mode(WIFI_OFF);
  delay(100);

  pinMode(inv_VLEDenable, OUTPUT);
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
  digitalWrite(D5Ledout, HIGH); // This is also the onboard LED. HIGH switched the LED off.
  digitalWrite(D6Ledout, LOW);
  digitalWrite(inv_VLEDenable, LOW); // Enable the outputs.

}

void loop()
{
  // Add a Led . Update(); for each LED you've added in the configuration section.

  ledA.Update(); // Add a line like this for every LED you added.
  ledB.Update(); // Add a line like this for every LED you added.
  ledC.Update(); // Add a line like this for every LED you added.
  ledD.Update(); // Add a line like this for every LED you added.
  ledE.Update(); // Add a line like this for every LED you added.
  ledF.Update(); // Add a line like this for every LED you added.
}
