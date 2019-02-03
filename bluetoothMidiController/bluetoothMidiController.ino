#include <Arduino.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "Adafruit_BLEMIDI.h"
#include "BluefruitConfig.h"
#include <Bounce2.h>

#define FACTORYRESET_ENABLE 1
#define MINIMUM_FIRMWARE_VERSION "0.7.0"

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

Adafruit_BLEMIDI midi(ble);

// buttons to initialize from left to right
const int NUMBER_OF_BUTTONS = 8;
int buttonPin[NUMBER_OF_BUTTONS] = {12, 11, 10, 2, 9, 18, 5, 3};

// track if a note is on for each button
bool buttonNoteOn[NUMBER_OF_BUTTONS] = {false, false, false, false, false, false, false, false};

// debouncer for buttons
Bounce debouncer[NUMBER_OF_BUTTONS];

// note and octave trackers; (OCTAVE * octaveMultiplier) + note = note in correct octave
const int OCTAVE = 12;
int note[NUMBER_OF_BUTTONS] = {0, 0, 12, 13, 14, 16, 17, 19}; // first two buttons are octave selections
int octaveMultiplier = 4;
const int MAX_NUMBER_OCTAVES = 7;

// delay time in milliseconds after sending MIDI message
const int DELAY_TIME = 5;

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
  
  // setup buttons
  for (int i=0; i<NUMBER_OF_BUTTONS; i++) {
    pinMode(buttonPin[i], INPUT);
    debouncer[i] = Bounce();
    debouncer[i].attach(buttonPin[i]);
    debouncer[i].interval(1);
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

  for (int i=0; i<NUMBER_OF_BUTTONS; i++) {
    // update the state of the button
    debouncer[i].update();

    // calculate the current note
    int currentNote = (OCTAVE * octaveMultiplier) + note[i];

    // read the button
    int button = debouncer[i].read();

    if (button == HIGH) {
       // first two buttons are the octave buttons
      if (i == 0){
        // lower an octave
        if (octaveMultiplier > 1) {
          octaveMultiplier -= 1;
        }
      } else if(i == 1){
        // raise an octave
        if (octaveMultiplier < 7) {
          octaveMultiplier += 1;
        }
      } else {
         
         // send note on/off based on button state
         if (buttonNoteOn[i] == false){
          
            // send the note on 
            midi.send(0x90, currentNote, 0x64);
            buttonNoteOn[i] = true;
           // delay(DELAY_TIME);
            Serial.print("Note: ");
            Serial.println(currentNote);
            
         } 
      }
    } else {
       // send note off
       midi.send(0x80, currentNote, 0x64);
       buttonNoteOn[i] = false;
       //delay(DELAY_TIME);
    }
  }
}
