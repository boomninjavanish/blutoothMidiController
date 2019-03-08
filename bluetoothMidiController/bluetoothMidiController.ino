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

// note and octave trackers; (OCTAVE * octaveMultiplier) + note = note in correct octave
const int OCTAVE = 12;

// track which octave we are in currently
int octaveMultiplier = 5;

// delay time in milliseconds after sending MIDI message
const int DELAY_TIME = 0;

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
   *    C1 - D#2 are on mcp0 pins 0-15
   *    E2 is on mcp1 pin 0
   *    OCT - and + are on mcp1 pins 4 and 5 respectively
   *    The RBG LED pins on mcp1 are: Red = 8; Blue = 7; Green = 6
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

  // bail if not connected
  if (! isConnected)
    return;
  
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
        }
      } else if(i == 15 && debouncer[i].fell()){
        // raise an octave
        if (octaveMultiplier < 8) {
          octaveMultiplier += 1;
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
