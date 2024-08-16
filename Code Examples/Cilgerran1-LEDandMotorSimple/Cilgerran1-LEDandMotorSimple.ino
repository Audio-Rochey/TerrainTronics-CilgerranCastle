/*
 8/16/2024 - This is a basic hardware test setup for the Cilgerran Castle board.
 Use this is make sure that your hardware is connected up correctly.
 It will cycle through Each LED, then spin the motor in each direction. Easy & Useful. :)

 Looking down on the board from above (USB connector from the wemos at the bottom), D1Ledout pins are on the left hand side.
 The Lower row of the output pins are all VLED. (5V or VBat that is switched on only when inv_VLEDenable is set LOW.
 The top row are the outputs from the transistors etc. Those outputs are LOW SIDE SWITCHING. 
 Low Side Switching means that you're connecting GROUND through the switch, not the supply voltage (like 5V). 
 Your LED's only care that the 5V is connected to the +'ve and that GND is connected to the -'ve output. 
 Whilst in your brain, you want to switch the positive supply, it's electronically much smaller and efficient to switch ground.
 See the github file for a wiring diagram! :)


*/


#define inv_VLEDenable D0
#define D1Ledout D5
#define D2Ledout D6
#define D3Ledout D7
#define D4Ledout D8
#define D5Ledout D4
#define D6Ledout D3
#define MotA D1
#define MotB D2

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  
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
  digitalWrite(inv_VLEDenable, LOW); // VLEDenable is ACTIVE LOW

}

// the loop function runs over and over again forever
void loop() {
  //ALL LEDS ON
  digitalWrite(D1Ledout, HIGH);
  digitalWrite(D2Ledout, HIGH);
  digitalWrite(D3Ledout, HIGH);
  digitalWrite(D4Ledout, HIGH);
  digitalWrite(D5Ledout, HIGH);
  digitalWrite(D6Ledout, HIGH);
  delay(500);
  //ALL LEDS OFF
  digitalWrite(D1Ledout, LOW);
  digitalWrite(D2Ledout, LOW);
  digitalWrite(D3Ledout, LOW);
  digitalWrite(D4Ledout, LOW);
  digitalWrite(D5Ledout, LOW);
  digitalWrite(D6Ledout, LOW);
  delay(1000);
  // Now Sequence Through them
  digitalWrite(D1Ledout, HIGH);
  digitalWrite(D2Ledout, LOW);
  digitalWrite(D3Ledout, LOW);
  digitalWrite(D4Ledout, LOW);
  digitalWrite(D5Ledout, LOW);
  digitalWrite(D6Ledout, LOW);
  delay(250);
  digitalWrite(D1Ledout, LOW);
  digitalWrite(D2Ledout, HIGH);
  digitalWrite(D3Ledout, LOW);
  digitalWrite(D4Ledout, LOW);
  digitalWrite(D5Ledout, LOW);
  digitalWrite(D6Ledout, LOW);
  delay(250);
  digitalWrite(D1Ledout, LOW);
  digitalWrite(D2Ledout, LOW);
  digitalWrite(D3Ledout, HIGH);
  digitalWrite(D4Ledout, LOW);
  digitalWrite(D5Ledout, LOW);
  digitalWrite(D6Ledout, LOW);
  delay(250);
  digitalWrite(D1Ledout, LOW);
  digitalWrite(D2Ledout, LOW);
  digitalWrite(D3Ledout, LOW);
  digitalWrite(D4Ledout, HIGH);
  digitalWrite(D5Ledout, LOW);
  digitalWrite(D6Ledout, LOW);
  delay(250);
  digitalWrite(D1Ledout, LOW);
  digitalWrite(D2Ledout, LOW);
  digitalWrite(D3Ledout, LOW);
  digitalWrite(D4Ledout, LOW);
  digitalWrite(D5Ledout, HIGH);
  digitalWrite(D6Ledout, LOW);
  delay(250);
  digitalWrite(D1Ledout, LOW);
  digitalWrite(D2Ledout, LOW);
  digitalWrite(D3Ledout, LOW);
  digitalWrite(D4Ledout, LOW);
  digitalWrite(D5Ledout, LOW);
  digitalWrite(D6Ledout, HIGH);
  delay(250);
  digitalWrite(D6Ledout, LOW);


  // NOW Test your motors
  digitalWrite(MotA, HIGH);
  digitalWrite(MotB, LOW);
  delay(3000);
  digitalWrite(MotA, LOW);
  digitalWrite(MotB, LOW);
  delay(300); // Delay with motor off to allow for spindown
  digitalWrite(MotA, LOW);
  digitalWrite(MotB, HIGH);
  delay(3000);
  digitalWrite(MotA, LOW);
  digitalWrite(MotB, LOW);
  delay(300); // Delay with motor off to allow for spindown
}
