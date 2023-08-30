#include <Arduino.h>

#include "ESP32TimerInterrupt.h"
#define TIMER_INTERVAL_MS       1000
volatile uint32_t lastMillis = 0;
ESP32Timer ITimer(0);
uint32_t timerCounter = 0;
bool IRAM_ATTR TimerHandler(void * timerNo) {
  timerCounter++;
}
void setup() {
  
  // Set up Serial Monitor
  Serial.begin(9600);
   if (ITimer.attachInterruptInterval(TIMER_INTERVAL_MS * 1000, TimerHandler))
  {
    lastMillis = millis();
    Serial.print(F("Starting  ITimer OK, millis() = ")); 
    Serial.println(lastMillis);
  }
  else 
    Serial.println(F("Can't set ITimer correctly. Select another freq. or interval")); 
 

}
 
void loop() {
  Serial.println ("Loop Triggered");
  Serial.print("Counter ");
  Serial.println(timerCounter);
  delay(2000);
}
