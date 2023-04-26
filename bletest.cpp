#include <Arduino.h>
#include <ArduinoBLE.h>
#include "OneButton.h"
#define BUTTON_VORNE_OBEN D8
const int button = BUTTON;
//Play = A#6
#define PLAY 94 
//Stop = A6
#define STOP 93
//Record = B6
#define RECORD 95
//Cursor Up C7
#define CURSOR_UP 96
// Cursor Down C#7
#define CURSOR_DOWN 97

OneButton ButtonVorneOben(button, true);

byte midiData[] = {0x80, 0x80, 0x00, 0x00, 0x00};

// set up the MIDI service and MIDI message characteristic:
BLEService midiService("03B80E5A-EDE8-4B33-A751-6CE34EC4C700");
BLECharacteristic midiCharacteristic("7772E5DB-3868-4112-A1A9-F2669D106BF3",
                                     BLEWrite | BLEWriteWithoutResponse |
                                     BLENotify | BLERead, sizeof(midiData));

void midiCommand(byte cmd, byte data1, byte  data2) {
  // MIDI data goes in the last three bytes of the midiData array:
  midiData[2] = cmd;
  midiData[3] = data1;
  midiData[4] = data2;

  midiCharacteristic.setValue(midiData, sizeof(midiData));
}

void click() {
  midiCommand(0x90, RECORD, 127);
}


void setup() {
  // initialize serial communication
  Serial.begin(9600);
  // initialize built in LED:
  // Initialize BLE:
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (true);
  }
  // set local name and advertised service for BLE:
  BLE.setLocalName("MIDI_BLE");
  BLE.setAdvertisedService(midiService);
  // add the characteristic and service:
  midiService.addCharacteristic(midiCharacteristic);
  BLE.addService(midiService);

  // start advertising
  BLE.advertise(); 
  Button.attachClick(click);
}

void loop() {
  Button.tick();
  delay(10);

}



