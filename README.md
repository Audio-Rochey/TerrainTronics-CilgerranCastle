# TerrainTronics-CilgerranCastle
6ch High Power LED Driver with Additional Motor driver
![Cilgerran Castle Intro Picture](https://github.com/user-attachments/assets/df3579ee-54e9-4fa5-a58d-67ecf89b8970)

## Why it exists and the problems it fixes
The Cilgerran castle was originally intended to drive a bunch of higher power LED's. Driving small LED's is quite simple, but as we move to long LED strings and the 1 watt and 3 Watt "Star LED's", driving them from smart sources, like Arduino's becomes harder and harder as the arduino's and microcontrollers like them aren't designed to drive high current and power output.

With 8 potential control signals coming up from the Wemos D1 Mini board that acts as the brain, 6 of them were given transistors to boost their power output and 2 of them have been set up to drive a DC motor birdirectionally. 

The danger with simply using transistors to drive high current/power outputs is that transistors don't have any way of limiting their power output. Pull too much power and they send themselves to the silicon heaven. This is solved on the Cilgerran Castle board by using a single electronic power switch that has built in short circuit protection. If any of the outputs become shorted, then they will all go off.  D0 is used to enable VLED. 

## What does a solution look like?
![VerticalStack](https://github.com/user-attachments/assets/601e2180-1c62-4092-af63-614ac53a7921)

A wemos D1 mini acts as the brain. It can no basic "on off" control for things like lightning effects, all the way through to fancy WiFi controls.
The pins pointing at the camera bring 6 high power LED outputs and 2 Motor Control outputs.


## Cool use cases
With it's ability drive 6 high power led's along with 2 more LEDs/ DC Motor from a smart source, existing code examples are available for:

### High Power Lightning Source

[Code Link](https://github.com/Audio-Rochey/TerrainTronics-CilgerranCastle/blob/d75bde7df7b1238a8ec136d23894cc93ea2bdc64/Code%20Examples/6chLightning.ino)

Some time ago, I made custom high power lightning effects for an entire tabletop of terrain that John at Tabletop Witchcraft was working on with the cool guys over at Wyrmwood Gaming! 
That solution used an off-the-shelf Arduino compatible DigiSpark board, an external transistor drive (Carew Castle) for single cloud. 
Using a Wemos D1 Mini along wiht this Carew Castle board now allows me to drive lightning strikes in up to 8 different clouds, all with randomized timings are behaviours. The code is available HERE:

### DMX Controller

[Code Link to come]

I'm addicted to the epic work done by Seb at Atmoseeker ([LINK](https://www.youtube.com/@Atmoseeker)). He controls all of his environmental lights using a stage lighting protocol called DMX. Many ceiling stage lights use DMX as their control method, along with a Wifi-variation called XYZ. 
DMX is designed for stage production where you have scenes, so you can do things like change from night to day. Imagine being able to change your ceiling lights from daylight to purples and blues and then having your tabletop buildings switch on their inhouse lights etc!
Example code (that needs further work!) is available here:

## Connecting up LED's and Motors.

### LED's
The LED outputs on the Cigerran Castle boards don't actually connect a high voltage on and off. It's the opposite. The VLED voltage is always present, and GND is switch on the other pin, on and off. This has no impact on the type of LED's that can be used, but be aware that there isn't a shared ground between the LED's. It's actually a shared VLED!
 
In addition, the outputs on the Cilgerran board don't have any kind of current limiting, so make sure that your LED's have a resistor in series with them. On a 1W Star LED, that looks like this: 
![61O0R89F-ULSmall](https://github.com/user-attachments/assets/8797add0-bdbb-4567-9392-5fdf9888335a)

-- on a pre-wired LED from Amazon, look for the lump in the line! If you're really unsure, you can actually drive a CONWY CASTLE board from each of these outputs.

> Make sure you're limiting the power for LED's - either use an external resistor, a Conwy Castle board or buy your 1W 5V LED's that have a Resistor already on them!

### MOTORS
The Cilgerran Castle board is designed to drive a single DC motor, such as N20 Motor. The A&B output pins are used directly to the motor. Use D1 and D2 to change direction of the Motor. Both pins can be PWM'd on the Wemos D1. However, the default Arduino implimentation on the D1 runs at 1kHZ - within Audio range. Some motors will make an audible tone. 

![61WdOWZzZNL](https://github.com/user-attachments/assets/dc514dba-0cb6-4780-8c35-1ec9c89bfed3)

> Where possible look for the a motor with the right RPM (Revolutions per minute) gearing for your target voltage (5V?) - for instance 6RPM at 5V would mean it takes 10 Seconds at full power to do a complete revolution. Getting slower speed using PWM (AnalogOut) will generate audible noise.

## What if you Need inputs?
The board is designed to use as many of it's pins as outputs. If however, you want to use on of the pins on the Wemos D1 mini as an INPUT instead of an output, then just make sure that the output that's normally connected to it, is not hooked up to an LED.
D1,2,5,6,7 can be used as inputs, however, make sure that the output is not connected to an LED etc. (the transistor will still be connected etc).

> Note: If D1 and D2 are both held HIGH (1) or both held LOW (0), the outputs from the MotorA and MotorB pins will be LOW.
Crossed out pins cannot be used as an input, as they have various functions at power on.

| LED Output Header Pin | Wemos D1 Pin          |
| --------------------- |-----------------------|
| LED Output 1          | D5 |
| LED Output 2          | D6 |
| LED Output 3          | D7 |
| ~LED Output 4~         | ~D8~ |
| ~LED Output 5~        | ~D4~ |
| ~LED Output 6~        | ~D3~ |
| MOTOR A               | D1 |
| MOTOR B               | D2 |

It's left as an excercise to the reader how you wish to connect to the appropriate pin. Check video's to see clever wiring hacks. 
An example of an input may be using a magnetic sensor to know when your motor has turned 360 degrees. In which case, you might choose Wemos Pin D5. D5 is connected to output channel 1, so don't use that outpin for an LED!

The Analog Input to the board, by default, is connected to the 5V rail through a 330K resistor. (R1 on the board). This is used to measure the power rail, in the case that it's a battery. More on that in the lipoly section.
If you don't care about monitoring the battery power supply, simply desolder R1, then connect whichever signal you want to A0.


## Hacks

### Power Efficiency: Power Converstion Losses
The Wemos D1 Mini is already knows as a pretty inefficient board. The device itself runs from a 3.3V supply, but the board is typically run from a 5V USB source. The wemos board uses a cheap linear regulator that burns off the difference of 1.7V.
Power = Current x Voltage, so if you're pulling 0.1A from the 3.3V supply, then the regulator has to drop 1.7V x 0.1A = 0.17Watts, just to maintain it's Wifi.
The alternative to linear regulators are called "buck converters" or dcdc converters. They switch the power and and off thousands of times a second to average out the voltage from 5V input to 3.3V output. This is WAAAY more power efficient, at the cost of circuit size and cost.
 
Modify your Wemos D1 mini to use the external dcdc convert by cutting this pin (Pin5 of the MA6211) on the board. Be aware that by doing so, your D1 Mini will never be able to run without a Cilgerran board sitting on top, or without a dab of solder connecting that linear regulator back in place.

![WemosCutTrace](https://github.com/user-attachments/assets/5db445fe-0b07-40c0-971e-4fd7a653d82f)


### Power Efficiency: Switch off Wifi if you aren't going to use it!

In Arduino, you can drop the power consumption significantly by adding the following lines in your SETUP function.

```
// Disable the Modem on the ESP8266/Wemos D1 Mini
WiFiMode(WIFI_STA);
WiFi.disconnect(); 
WiFi.mode(WIFI_OFF);
delay(100);
```


These lines drop the current consumption from about 80mA to 14mA.

## Using Batteries

### Running Directly from an External battery
There are two pads on the Cilgerran Castle board. They can be connected to a 4.5V Alkaline battery pack (such as a 3xAA battery pack) or a Lithium Ion (LiPoly) battery. It's recommended that you use an external power switch - one that is built into the battery holder.
![Battery Connectors](https://github.com/user-attachments/assets/cec718d3-8fbc-4914-af9e-7b3c34eca3b9)

USB can be connected to the Wemos board for programming whilst the battery is connected, as a diode is in place to isolate the battery pack if a high voltage source (USB) is present.

### Considerations for running from a LiPoly Cell.
Should you run the setup from an external USB Power bank, then simply connect the USB-Micro or USB-C cable to your Wemos d1 as you usually would. Every USB power bank on the planet has a little boost circuit that will provide a consistent 5V USB Power supply to your setup. 
However, not all setups have enough room for such a circuit!
Rechargable, Lithium Ion Battery cells are available from Amazon and other places that can be used. There are two pads on the Cilgerran board that the batteries can be connected to. 

For rechargable batteries, I suggest using a wired connector, such as: - that will then allow you to "unplug" the battery, and recharge it with a separate charge circuit when it's power runs too low. (https://a.co/d/9qAoYhL)
> Note - always make sure the polarity of your battery connector matches the polarity of the wire going to the Cilgerran board. 

LiPoly batteries provide a battery voltage that starts at about 4.2V when fully charged, and can run down to 2.7V. If they are run lower, then you can do some permanent damage to the batteries. It's important that you limit the power draw to as little as possible once you reach 2.7V.
I strongly suggest using a little code that checks the battery voltage and if it's below 2.8V, putting the entire board into shutdown/standby. (thus switching off the LED's, Motors).
At the start of each loop, simply execute the following code:
Insert Code Here:

That code actually makes sure that the voltage doesn't go below 3.3V - giving you plenty of safe operating voltage.

## How do I buy it?

Cilgerran Castle board comes as both a single board (with connectors) and as a kit, with additional components to make a system.

### What's in the standalone board?
- 1x Cilgerran Castle Board
- 1x 2x8 Right Angle Connector
  
### What's in the Kit?

- 1x Cilgerran Castle Board
- 1x Wemos D1 Mini - preprogrammed for lightning effects.
- 4x Low Profile connectors
- 1x 2x8 Right Angle Connector
- 1x 2x8 IDC Connector
- 1x 1M 16pin Rainbow Wire
- 6x Bright White LED's

## Schematics

![CilgerranPG2p0Schematic](https://github.com/user-attachments/assets/079c2b2d-b7ee-416d-8114-96ad34fee477)
