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

****** Video Specific *******
Video Link: https://www.youtube.com/watch?v=xORPmeiwO_I

The D1 output of the Cilgerran board is connected to a Green 1W STar LED. It needs to be driven with a candle patter.
"WeatherVane" code was added. It rotates the motor every minute or so.
"MordheimMode" is a switch connected to A0. If the switch is closed (and shorting hte signal to GND) then the code knows to switch ON the LED Flicker.
This allows the Church to be used for games without green Wyrdstone! :)




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



class WeatherVane
{
  // State comments below are work in progress
  // Flag FlagA(shakedelay, actuatortime, D1, D2, 0 (directpin));
  /* Passed Variables: speed, actuator time, motA and motB globals, DirectPin or Variable? 
  actuatortime is actually the RPM of a DC motor.
  (directpin sets up the GPIO pins, variable allows to write to a global variable, eg i2c expander)
  Created and tracked variables: remainingWobbleCount, delayTillNextState
  */ 
  
  
  // Class Member Variables
  // These are initialized at startup
  int MotAOut;      // Output of MotA (could be a pin or a global variable
  int MotBOut;      // Output of MotB (could be a pin or a global variable
  int speed; // how long between shakes // int is 16 bits, 32 seconds.
  int actuatorTime; // RPM of the motor
  bool pinOrVariable; // FALSE = Variable, TRUE = PIN
  

  // These maintain the current state
  int state;                      // Define the state (0 = change to new angle, 1 = Stop turning, 2 Wiggle Left, 3 Flash Right)
  unsigned long delayTillNextState;        // delay between states
  unsigned long previousMillis;   // will store last time LED was updated
  int remainingWobbleCount; // how many wobbles left to go?
  int directionfornewangle; // which direction should we turn for the new direction?
  unsigned long timeTillNewAngle; // how long until the next change in direction
  unsigned long turningTimeForFortyFive; // how long to turn until we get to 45 degrees?

  // Constructor - creates a candle flicker
  // and initializes the member variables and state
  public:
  WeatherVane(int in_MotAOut, int in_MotBOut, bool in_pinOrVariable, int in_speed, int in_actuatorTime)
  {
  
  MotAOut = in_MotAOut;
  MotBOut = in_MotBOut;
  pinOrVariable = in_pinOrVariable;
  if (pinOrVariable == 1){
	pinMode(MotAOut, OUTPUT); 
	pinMode(MotBOut, OUTPUT); 
  }
  speed = in_speed;
  actuatorTime = in_actuatorTime;
  remainingWobbleCount = 0;
  turningTimeForFortyFive = ((60000/8) / actuatorTime); // 1 revolution time is 60/RPM. 45 degrees is 1/8th of a full circle, so we need to caluation 1/8th of 60/RPM. Same as 60/8 *
  Serial.print("TurningTimeFor45=  "); //debug
  Serial.println(turningTimeForFortyFive);
  
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
      Serial.print("UPDATE Weathervane STATE"); //debug
      Serial.println(state);
      
        if (currentMillis > timeTillNewAngle){
          state = 0;
        }
        switch (state){
        case 0: // Decide which way to move (random) and how long to turn for (any time less than 270 degrees of a new turn.)
          directionfornewangle = random (0,2); 
          Serial.print("UPDATE Weathervane Direction"); //debug
          Serial.println(directionfornewangle);
          if (directionfornewangle == 0){
            Serial.println("Turning Left"); //debug
            digitalWrite(MotA, HIGH); 
			      digitalWrite(MotB, LOW); 
          }
          else {
            Serial.println("Turning Right"); //debug
            digitalWrite(MotA, LOW); 
			      digitalWrite(MotB, HIGH); 
          }
		      state = 1;
          timeTillNewAngle = (currentMillis + (random(180000, 240000))); // how long until we go back to state 0? // debig is 18 to 24 seconds, add zero's to make 3 to 4 mnutes
          delayTillNextState = turningTimeForFortyFive; // how long till we stop moving?
        break;
          
        case 1: // stop turning to your new angle
          digitalWrite(MotA, LOW); 
			    digitalWrite(MotB, LOW);

          state = 0;
          delayTillNextState = 60000; 
          break;
          
        case 2: // Wiggle Left
          analogWrite(MotA,55);
          //digitalWrite(MotA, HIGH); 
			    digitalWrite(MotB, LOW);
          state = 3;
          delayTillNextState = 750;
          break;
		
		    case 3: // Wiggle Right
          digitalWrite(MotA, LOW); 
			    //digitalWrite(MotB, HIGH);
          analogWrite(MotB, 55);
          state = 2;
          delayTillNextState = 750;
          break;
		  
		 case 4: // Dummy State
          state = 0;
          delayTillNextState = 0;
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

Candle ledA(D1Ledout);
WeatherVane Chicken(D1 ,D2, 0, 100, 3);
//Lightning ledC(D3Ledout);
//Lightning ledD(D4Ledout);
//Lightning ledE(D5Ledout);
//Lightning ledF(D6Ledout);


int mordheimMode = A0;
int mordheimValue = 0;  // variable to store the value coming from the sensor

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

  while (!Serial) {
        // wait for Serial to become active
    }
  Serial.println("Serial Link Complete:");

  pinMode(inv_VLEDenable, OUTPUT);
  pinMode(MotA, OUTPUT); // CRITCAL 
  pinMode(MotB, OUTPUT);
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

	digitalWrite(D2Ledout, HIGH); // ON for the Neve LED

}

void loop()
{
  // Add a Led . Update(); for each LED you've added in the configuration section.

  //Serial.println(analogRead(mordheimMode));
  delay(1);
  if (analogRead(mordheimMode) >> 1000){
    ledA.Update(); // Add a line like this for every LED you added.
  } 
  else {
    digitalWrite(D1Ledout, LOW);
  } 
  Chicken.Update();
  //ledB.Update(); // A dd a line like this for every LED you added.
  //ledC.Update(); // Add a line like this for every LED you added.
  //ledD.Update(); // Add a line like this for every LED you added.
  //ledE.Update(); // Add a line like this for every LED you added.
  //ledF.Update(); // Add a line like this for every LED you added.
}
