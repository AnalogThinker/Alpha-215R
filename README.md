# Alpha-215R
"Alpha 215" Big Dot LED display modernized - ESP8266 to MQTT

This is a small project to revive an old Big Dot LED display, so you can push messages to it from MQTT (mainly used for Home Assistant message push for me).

# Hardware
You will need :
- Alpha sign
- A DPDT switch
- ESP8266 (I used a WEMO D1 Mini ESP8266)
- MAX3232 module (for level shifting to TTL)
- Wires

On the Alpha sign:
I am using the RS232 input (the right RJ12, labelled RS485/RS232 INPUT).
Page 28 of this document shows the pinout: https://www.alpha-american.com/alpha-manuals/M-Networking-Alpha-Signs.pdf
Pins 3/4 are for RX/TX
Pins 2/5 are for RS485 -/+
Pins 1/6 are VCC/GND

On the ESP8266:
I added a DPDT switch to break TX and VCC when I program it. I also added a diode for TX (optional).
Although a lot of projects out there are connecting both RX/TX, I only used the GPIO pin marked "TX" as I don't care about RX messages.

So to recap:
- Alpha VCC -> DPDT -> ESP8266 "5V" -> MAX3232 VCC
- Alpha GND -> ESP8266 "GND" -> MAX3232 GND
- ESP8266 TX -> (Optional diode) -> DPDT -> MAX3232 "RS232 Input" (Arrow from the RS232 side pointing towards the module)
- MAX2323 module "TTL TX OUTPUT" (Arrow from the TTL side pointing outward) -> Alpha RX 

# Programming the ESP8266
I used Visual Studio Code.
Create a new platformIO project, select your board (I used WEMO D1 Mini ESP8266).
Copy the content of platformio.ini in your project
Copy the content of main.cpp in your project

Adjust :
- SSID and WiFi password
- MQTT Login/Password

# On Home Assistant (NodeRed)
I used NodeRed to send an MQTT message to the display (Assuming you already configured your MQTT broker, and have it setup in NodeRed)
Create 2 MQTT Out messages with the following topics:
- alpha215/message
- alpha215/effect

The Message card will take any paylod from its input and push it to the display.

The Effect card will apply a display effect if you want to.

# Effects
I used this protocol document as a base for this project.
Interesting sections are 6.1.1 (Write TEXT file) on page 18, and 7.9.1 (Standard modes) on page 89.
By default, message are sent using static centered display (0x62).
You can modify the next message's effect by sending any of the effect code before your message (format in NodeRed payload would be 0x62 for "HOLD", Alpha document has it as "62H".

# Creativity is your limit!
Feel free to play, edit, branch!
You can easily make the display draw pixel art and do whatever you want!
