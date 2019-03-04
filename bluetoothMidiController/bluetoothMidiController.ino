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

// setup the bluetooth to only work with a certain firmware version
//  update the firmware via the Adafruit Bluefruit LE Connect app on iOS and Android
#define FACTORYRESET_ENABLE 1
#define MINIMUM_FIRMWARE_VERSION "0.7.0"

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

Adafruit_BLEMIDI midi(ble);

// define and init the MCP 23017 port expander objects
Adafruit_MCP23017 mcp[2];
#define addr1 0x00 // mcp0 I2C address
#define addr2 0x01 // mcp1 I2C address

// track if a note is on for each button
bool buttonNoteOn[2][16];

// note and octave trackers; (OCTAVE * octaveMultiplier) + note = note in correct octave
const int OCTAVE = 12;

/* map the MIDI note number to each button on each extender
 *  a zero indicates that it is not a button; 
 *  4 and 5 map to the OCT - and + buttons respectively 
 */
int note[2][16] = {
                    {12, 13, 14, 16, 17, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29},
                    {30,  0,  0,  0,  4,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}
};

// track which octave we are in currently
int octaveMultiplier = 4;
const int MAX_NUMBER_OCTAVES = 7;

// delay time in milliseconds after sending MIDI message
const int DELAY_TIME = 10;

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
  mcp[0].begin(addr1);
  mcp[1].begin(addr2);
  
  /* Setup the buttons and LED
   *  
   *  All of the buttons are connected to both 23017 chips
   *    C1 - D#2 are on mcp0 pins 0-15
   *    E2 is on mcp1 pin 0
   *    OCT - and + are on mcp1 pins 4 and 5 respectively
   *    The RBG LED pins on mcp1 are: Red = 8; Blue = 7; Green = 6
   */
  for (int i = 0; i < 2; i++){
    for (int j = 0; j < 16; j++){

      // initialize the button pressed tracker to false
      buttonNoteOn[i][j] = false;
      
      int currentPin = note[i][j];
      
      // set pins for LED to OUTPUT and set debouncer for buttons
      if ( currentPin == 4 || currentPin == 5 ){
        mcp[i].pinMode(j, OUTPUT);
      } else {
        mcp[i].pinMode(j, INPUT);
        mcp[i].pullUp(j,  HIGH);
      }
    }
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
  for (int i=0; i<2; i++) {
    for (int j=0; j<16; j++) {      
      // calculate the current note
      int currentNote = (OCTAVE * octaveMultiplier) + note[i][j];
  
      // read the buttons
      int buttonState = mcp[i].digitalRead(j);

      // if button is HIGH send note on; if LOW, send note off
      if (buttonState == HIGH) {
        
         // check if the button being pressed is an octave button
        if (note[i][j] == 4){
          // lower an octave
          if (octaveMultiplier > 1) {
            octaveMultiplier -= 1;
          }
        } else if(note[i][j] == 5){
          // raise an octave
          if (octaveMultiplier < 7) {
            octaveMultiplier += 1;
          }
        } else {
             
           // send note on/off based on button state
           if (buttonNoteOn[i][j] == false){
            
              // send the note on 
              midi.send(0x90, currentNote, 0x64);
              buttonNoteOn[i][j] = true;

              digitalWrite(13, HIGH); // led on
              
              delay(DELAY_TIME);
              Serial.print("Note: ");
              Serial.println(currentNote);
              
           } 
        }
      } else {
         // send note off
         midi.send(0x80, currentNote, 0x64);
         buttonNoteOn[i][j] = false;

         digitalWrite(13, LOW); // led off
         
         delay(DELAY_TIME);
      }
    }
  }
}
