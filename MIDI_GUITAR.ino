
/*
Video for this project here:https://youtu.be/QPgYpVHFSQY

Thank you for checking out my video and my code!

For more videos and information check out
https://www.youtube.com/merwinmusic
http://www.lukemerwin.com/

*/

#include <Arduino.h>
#include <SPI.h>

#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "Adafruit_BLEMIDI.h"

#include "BluefruitConfig.h"

#define FACTORYRESET_ENABLE         0
#define MINIMUM_FIRMWARE_VERSION    "0.7.0"


Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

Adafruit_BLEMIDI midi(ble);

bool isConnected = false;


// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

// callback
void connected(void)
{
  isConnected = true;

  Serial.println(F(" CONNECTED!"));
  delay(1000);

}

void disconnected(void)
{
  Serial.println("disconnected");
  isConnected = false;
}

void BleMidiRX(uint16_t timestamp, uint8_t status, uint8_t byte1, uint8_t byte2)
{
  Serial.print("[MIDI ");
  Serial.print(timestamp);
  Serial.print(" ] ");

  Serial.print(status, HEX); Serial.print(" ");
  Serial.print(byte1 , HEX); Serial.print(" ");
  Serial.print(byte2 , HEX); Serial.print(" ");

  Serial.println();
}

                                

const int ButtonPins[15] = {2, 3, 5, 6, 9, 10, 11, 12, 13, 18, 19, 20, 21, 22, 23};

int LastButtonState[15] = {0};
int currentMillis = 0;
int LastButtonChangeTime[15] = {currentMillis};

int NoteNumber[13] = {24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36};


int buttonPushCounter = 0;



void setup(void)
{
  
                                        
  
 

  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit MIDI Example"));
  Serial.println(F("---------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

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

  //ble.sendCommandCheckOK(F("AT+uartflow=off"));
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  /* Set BLE callbacks */
  ble.setConnectCallback(connected);
  ble.setDisconnectCallback(disconnected);

  // Set MIDI RX callback
  midi.setRxCallback(BleMidiRX);

  Serial.println(F("Enable MIDI: "));
  if ( ! midi.begin(true) )
  {
    error(F("Could not enable MIDI"));
  }

  ble.verbose(false);
  Serial.print(F("Waiting for a connection..."));

for(int i=0; i < 15; i++)
  pinMode(ButtonPins[i],INPUT_PULLUP);

}

void loop() {
  //-------------------------------------------------------------------------------------//
  // interval for each scanning ~ 500ms (non blocking)
  ble.update(250);

  // bail if not connected
  if (! isConnected)
    return;

int octave = (buttonPushCounter * 12);

//-----------------------------------------------------------------------------------//
   {
  unsigned long currentMillis = millis();
  for (int i=0; i<15; i++) {
    boolean state = !digitalRead(ButtonPins[13]);  // 'true'==pressed, 'false'==released
    // Check for state change and do debounce
    if (state != LastButtonState[13] && currentMillis-LastButtonChangeTime[13] > 50) {
      LastButtonState[13] = state;
      LastButtonChangeTime[13] = currentMillis;
      if (state){
        buttonPushCounter++;
      Serial.println("on");
      Serial.print("number of button pushes:  ");
      Serial.println(buttonPushCounter);
    } else {
      // if the current state is LOW then the button
      // wend from on to off:
      Serial.println("off");
      }
    }
  }
   }
//-----------------------------------------------------------------------------------//
   {
  unsigned long currentMillis = millis();
  for (int i=0; i<15; i++) {
    boolean state = !digitalRead(ButtonPins[14]);  // 'true'==pressed, 'false'==released
    // Check for state change and do debounce
    if (state != LastButtonState[14] && currentMillis-LastButtonChangeTime[14] > 50) {
      LastButtonState[14] = state;
      LastButtonChangeTime[14] = currentMillis;
      if (state){
        buttonPushCounter--;
      Serial.println("on");
      Serial.print("number of button pushes:  ");
      Serial.println(buttonPushCounter);
    } else {
      // if the current state is LOW then the button
      // wend from on to off:
      Serial.println("off");
      }
    }
  }
   }
{
  if (buttonPushCounter < -1){
    buttonPushCounter = -1;
  }
  if (buttonPushCounter > 6){
    buttonPushCounter = 6;
  }
}
  
//-----------------------------------------------------------------------------------//
   {
  unsigned long currentMillis = millis();
  for (int i=0; i<13; i++) {
    boolean state = !digitalRead(ButtonPins[i]);  // 'true'==pressed, 'false'==released
    // Check for state change and do debounce
    if (state != LastButtonState[i] && currentMillis-LastButtonChangeTime[i] > 50) {
      LastButtonState[i] = state;
      LastButtonChangeTime[i] = currentMillis;
      if (state)
        midi.send(0x90, octave + NoteNumber[i], 0x64);  // Pressed
      else
        midi.send(0x80, octave + NoteNumber[i], 0x64);  // Released
      }
    }
  }
//-----------------------------------------------------------------------------------//

}           
