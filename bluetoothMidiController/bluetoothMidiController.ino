/*
 * bluetoothMidiController.ino
 * 2019 Matthew Dunlap
 * 
 * This project utilizes an Adafruit Feather 32u4 Bluefruit LE board (https://www.adafruit.com/product/2829)
 * to allow a user to input notes on a piano-style keyboard wirelessly to a mobile phone or tablet. 
 * Currently, as per my testing, MIDI over bluetooth LE is best supported on a device running iOS. However,
 * it is my hopes that this will change in the future.
 *
 */

#include <Arduino.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "Adafruit_BLEMIDI.h"
#include "Adafruit_MCP23017.h"
#include "BluefruitConfig.h"
#include "Bounce2mcp.h"

// setup the bluetooth to only work with a certain firmware version
//  update the firmware via the Adafruit Bluefruit LE Connect app on iOS and Android
#define FACTORYRESET_ENABLE 1
#define MINIMUM_FIRMWARE_VERSION "0.7.0"

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

Adafruit_BLEMIDI midi(ble);

// define and init the MCP 23017 port expander objects
Adafruit_MCP23017 mcp;

//debouncer for the buttons
BounceMcp debouncer[16];

// track if a note is on for each button
bool buttonNoteOn[16];

// track if octave light has been turned on
bool lightOn = false;

// note and octave trackers; (OCTAVE * octaveMultiplier) + note = note in correct octave
const int OCTAVE = 12;

// track which octave we are in currently
int octaveMultiplier = 5;

// delay time in milliseconds after sending MIDI message
const int DELAY_TIME = 0;

// RGB LED pins
const int RED_PIN = 10;
const int GREEN_PIN = 11;
const int BLUE_PIN = 12;

// track if Bluetooth Module is connected
bool isConnected = false;


// error handeling
void error(const __FlashStringHelper*err) {
	Serial.println(err);
	while (1);
}

// when connected
void connected(void){
	isConnected = true;
	Serial.println(F(" CONNECTED!"));
	delay(1000);
}

// when disconnected
void disconnected(void){
	Serial.println("DISCONNECTED!");
	isConnected = false;
}

void setup() {
  Serial.begin(115200);

  // setup the built-in light
  pinMode(13, OUTPUT);
  
  // setup the port expanders
  mcp.begin();
  
  /* Setup the buttons and LED
   *  
   *  All of the buttons are connected to both 23017 chips
   *    C1 - C2 are on mcp pins 0-12
   *    OCT - and + are on mcp pins 14 and 15 respectively
   *    An optional button is available on mcp pin 13
   *    The RBG LED pins are on the microcontroller: 
   *          Red = A0; Blue = A1; Green = A2
   */
  for (int i = 0; i < 16; i++){
    // initialize the button pressed tracker to false
    buttonNoteOn[i] = false;
    
    // setup pins as input
    mcp.pinMode(i, INPUT);
    mcp.pullUp(i,  HIGH);

    // setup debouncer if input
    debouncer[i] = BounceMcp();
    debouncer[i].attach(mcp, i, 5);   
  }

  // setup LED
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  
  // setup bluetooth
  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ) {
      error(F("Couldn't factory reset"));
    }
  }
   ble.echo(false);

   Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();
  
  /* Set BLE callbacks */
  ble.setConnectCallback(connected);
  ble.setDisconnectCallback(disconnected);

  Serial.println(F("Enable MIDI: "));
  if ( ! midi.begin(true) )
  {
    error(F("Could not enable MIDI"));
  }
  
  ble.verbose(false);
  
}

void loop() {

  // interval for each scanning ~ 500ms (non blocking)
  ble.update(1);

  // bail if not connected; turn light on for the first time
  if (! isConnected) {
    octaveColor(0); // turn light off
    lightOn == false;
    return;
  } else if (! lightOn){
    octaveColor(octaveMultiplier); // turn light on
    lightOn == false;
  }
    
  
  pollButtons();

}

void pollButtons(){
  for (int i=0; i<16; i++) {  
      // calculate the current note
      int currentNote = (OCTAVE * octaveMultiplier) + i;
  
      // read the current button
      debouncer[i].update();
      
      // check if the button being pressed is an octave button
      if (i == 14 && debouncer[i].fell()){
        // lower an octave
        if (octaveMultiplier > 1) {
          octaveMultiplier -= 1;
          octaveColor(octaveMultiplier);
        }
      } else if(i == 15 && debouncer[i].fell()){
        // raise an octave
        if (octaveMultiplier < 8) {
          octaveMultiplier += 1;
          octaveColor(octaveMultiplier);
        }
      } else if( i < 14 ){ // button is piano key
        int buttonState = debouncer[i].read();
        if ( buttonState == LOW && buttonNoteOn[i] == false){  
          // send the note on 
          midi.send(0x90, currentNote, 0x64);
          buttonNoteOn[i] = true;

          digitalWrite(13, HIGH); // led on
          
          delay(DELAY_TIME);
          Serial.print("Note: ");
          Serial.println(currentNote);
        } else if (buttonState == HIGH && buttonNoteOn[i] == true) {
          // send note off
          midi.send(0x80, currentNote, 0x64);
          buttonNoteOn[i] = false;
    
          digitalWrite(13, LOW); // led off
        }
      } 
   }
}

void setColor(int red, int green, int blue){
  digitalWrite(RED_PIN, red);
  digitalWrite(GREEN_PIN, green);
  digitalWrite(BLUE_PIN, blue);
}

void octaveColor(int octaveMultiplier){

  switch (octaveMultiplier){
    case 1 :
      setColor(HIGH, LOW, LOW); // red

    case 2 :
      setColor(HIGH, HIGH, LOW); // yellow x
      
    case 3 :
      setColor(LOW, HIGH, LOW); // green
      
    case 4 : 
      setColor(HIGH, HIGH, HIGH); // white x
      
    case 5 : 
      setColor(LOW, HIGH, HIGH); // aqua

    case 6 : 
      setColor(HIGH, LOW, HIGH); // purple

    case 7 :
      setColor(LOW, LOW, HIGH); // blue

    default :
      setColor(LOW, LOW, LOW); // off
    
  }
}
