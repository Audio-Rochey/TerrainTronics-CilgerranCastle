/*
8/18/24 - Dafydd Roche

Gravestone shaker simply toggles the Motor Driver to flip a hand wound actuator.
You can also use a small solenoid for this, such as https://a.co/d/5Q3j3Cf
Or a micro servo like: https://a.co/d/9rrgtYn

The motor spins when A is high and B is low, or the opposite. 
motors and solenoids change direction when electricity flows in a different direction through them.
to achieve this, the Cilgerran board uses something called a H Bridge, where you can switch 
which direction the electricity flows through the motor coil.

inv_VLEDenable is a signal thats used with an electronic power switch on the chip. 
When this pin is pulled low, it switches on the VLED. A power source to the motors and LED's.

So when both MOTA and MOTB are high or low, the motor doens't spin.

*/

//Define the pins on Cilgerran
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
  pinMode(inv_VLEDenable, OUTPUT);
  pinMode(MotA, OUTPUT);
  pinMode(MotB, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(inv_VLEDenable, LOW);
  digitalWrite(MotA, LOW);  // 
  digitalWrite(MotB, LOW);  // 
  
  for(int i=0; i<(random(2,5)); i++) { // how many times will it wobble back and forth? it counts up between 0 and some number between 2 and 5.
  int brightness = 90;
  int shakedelay = 175;
  int actuatortime = 5;
  
  // Wobble in one direction for actuatortime amount of time. (no need to keep pushing once the stone has wobbled over)
  digitalWrite(inv_VLEDenable, LOW); // switch on 5V
  digitalWrite(MotA, HIGH);  // turn one way for actuator time, then reset
  digitalWrite(MotB, LOW);  // 
  delay(actuatortime);
  
  //Now wait a while before rocking back
  digitalWrite(MotA, LOW);  // makes both A nd B = LOW, so no power is burned in the motor, nor does it burn
  delay(shakedelay);                      // wait for a second

  // Wobble back the other way
  digitalWrite(MotB, HIGH);  // turn the other way for actuator time, then reset
  digitalWrite(MotA, LOW);  // 
  delay(actuatortime);

  // Now wait until you loop around.
  digitalWrite(MotB, LOW);  // 
  delay(shakedelay);                      // wait for a second
  }
  
  // Now a long wait between shakes
  digitalWrite(MotA, LOW);  // all off
  digitalWrite(MotB, LOW);  // all off
  digitalWrite(inv_VLEDenable, LOW); // all off
  delay(random(1000,2000)); // wait between 1 or 2 seconds
}
