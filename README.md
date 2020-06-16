# portaMid
![portaMid without phone docked](https://github.com/boomninjavanish/media/raw/master/portaMid/portaMidFinal.JPEG)

[Click to view video of portaMid in use.](https://vimeo.com/428870643)

[Click here to download the manual.](https://github.com/boomninjavanish/portaMid/raw/master/PortaMidUserManual.pdf)

The portaMid keyboard is a MIDI keyboard that works over the Bluetooth Low Energy (BLE) protocol which allows one to control music apps on any  Apple iOS device. The device contains a 500 mAh lithium polymer rechargeable battery which lasts for many hours of use. 

The one octave of piano style keys are soft to the touch but contain tactile feedback so that the user knows when the key has been pressed. Utilizing the octave  keys, the user may lower or raise the octave in which the piano-style keys play. The LED represents the octave selection via different color output.

A removable phone mount allows for use on the go for different sizes of iPhone. In fact, every iPhone since the 4S is supported by the device. Most music apps support MIDI over BLE natively and connect to the portaMid with no configuration required: just connect and use. An example of set-up with the GarageBand app may be found in the user manual.

## Motivation
I chose to create a portable MIDI controller that will allow one to dock a phone and use like a gaming controller. Often, I create music on train trips using my phone and hate inputting notes on the tiny keyboards presented on screen.

## Creation
### Board
A circuit board was designed using KiCAD and is centered around the Adafruit Feather 32u4 Bluefruit LE development microcontroller. The PCB design may be sent to a company such as OSH Park or PCB Way to be printed. The following is a bill of materials for the project (sans circuit board and case):

QTY | Part
---- | ------
1 | Adafruit Feather 32u4 Bluefruit LE Microcontroller
1 | Microchip MCP2307 I2C I/O expander (SPDIP)
1 | Adafruit NeoPixel Through-Hole 5mm LED
13 | 12mm tactile switches
3 |  5mm tactile switches
1 | 100nF ceramic capacitor
1 | 2mm DPDT surface mounted slide switch (DigiKey PN: 401-2002-1-ND)
1 | 1K SMD 1206 resistor
1 | 4.7K SMD 1206 resistor
1 | 3.7V/500mAh Lithium Ion Polymer Battery with 2-pin JST-PH connector

### Case
The final case was modeled in Fusion 360 and was designed to accept an existing phone mount that was found in [Thingiverse](https://www.thingiverse.com/thing:1313336). In order to ensure that the dimensions were correct, the board was exported from KiCAD as a STEP file. Other components such as switches, USB ports, and LEDs were imported into Fusion from a website called [GrabCAD](https://grabcad.com). This website is a community website similar to Thingiverse, but contains mostly engineering components. 

I printed the buttons in NinjaFlex 85A TPU,  which leaves the buttons tactile but feeling soft and reduces noise. The phone clamp connects to the case utilizing a ball joint which allows the keyboard to be used without the phone attached.

The side of the case contains an indentation that allows a USB micro cable to be plugged into the Adafruit feather for charging. The charging LED is visible through one hole near the port and a programmable LED is visible through another.

## User Feedback
For testing, the portaMid was given to two people for a week each. The following is the feedback from the first person:

>*I love the PortaMid!! As a musician who uses midi for all of my projects, it’s so convenient to be able to have an external midi controller. Especially because it’s so portable! I can easily fit it into my purse or backpack and use it wherever I go, train, bus, or at home. I also really love the design, the PortaMid looks as fun as it is to use. The buttons are accessible and labeled to help easily find the functions. I can’t wait to have a PortaMid of my own!*

The second person had the following to say:
>- *Good size to hold*
>- *Clear and simple user manual*
>- *LOVE the color change for different octaves- that was very useful*
>- *Bluetooth easy to set up*
>- *It only took me 10-15 min to get the hang of it and start playing melodies*
>- *I was really excited about the product, and it was more fun to use than I thought it would be*
>- *Thank you for letting me test it!*

## Improvements and Future Work
Going forward, further refinements to the device will be made. For example, it would be nice to have buttons that have the tactile feeling but are less difficult to press and create less noise. Research needs to be done on what could replace the standard tactile buttons with something that is better. In addition, it would be nice to have some velocity sensitive keys for playing more than one dynamic.

In addition, I will need to create a circuit that can poll the battery level at intervals in order to indicate the current charge level or if the battery is dying.

The stand could use a wing-nut to allow the user to adjust the tension so as to keep the unit in the proper position when in use. Also, it would allow for ease of removing the stand entirely.

While using the product, I found myself gripping the device like a game-pad. I had the thought of incorporating shoulder buttons which could be used for modulation, pitch bend, after-touch pressure, etc. 

I envision an entire line of these being made with different properties. For example it would be nice to have a version that mimics the Moog style ribbons, or another version that is a mini sequencer. It would only take finding different sensors or button layouts and incorporating them into the current design for a completely different type of interface.
