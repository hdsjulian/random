#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#define SET_TIME_OFFSET 1
#define PRINT_MODE -1
#define WAIT_FOR_INSTRUCTIONS 0
#define CALIBRATE 2
//Network
//uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t broadcastAddress[] = {0xB4,0xE6,0x2D,0xE9,0x3C,0x21};
esp_now_peer_info_t peerInfo;


//Variables for Time Offset
typedef struct timer_message {
  uint8_t messageType;
  uint16_t counter;
  uint16_t sendTime;
  uint16_t lastDelay;
} timer_message;

//3 ints 1 float 1 address 1 string 1 bool?

typedef struct mode_change {
  uint8_t messageType;
  uint16_t mode;
} mode_change;


int messageArriveTime;
int lastTime = 0;

//message_struct testmsg;
int timeOffset;

//general setup
int mode = SET_TIME_OFFSET;


typedef struct timer_message {
  uint8_t messageType;
  uint16_t counter;
  uint16_t sendTime;
  uint16_t lastDelay;
} timer_message;


void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  switch (incomingData[0]) {
    case 0: 
      timer_message timerMessage = static_cast<timer_message>(&incomingData[0]);
      Serial.println(timerMessage.counter);
      break;
    default: 
      Serial.println("Data type not recognized");
  }
  /*
  if (mode == SET_TIME_OFFSET) {
    messageArriveTime = micros();
    memcpy(&timerMessage, incomingData, sizeof(timerMessage));
      if (abs(messageArriveTime - lastTime-1000000) < 300 and abs(timerMessage.lastDelay) <1500) {
        timeOffset = lastTime-oldMessage.sendTime;
        mode = PRINT_MODE;
      }
      else {
        lastTime = messageArriveTime;
        oldMessage = timerMessage;
      }
  }
  if (mode == 0 )
  */
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t sendStatus) {
  messageArriveTime = ESP_NOW_SEND_SUCCESS ? micros() : 0;
}


void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
    // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);  
  esp_now_register_send_cb(OnDataSent);

}

void loop() {
  if (mode == -1 ){
    mode = 0;
    Serial.print("Time: ");
    Serial.println(timerMessage.sendTime);
    Serial.print("Counter: ");
    Serial.println(timerMessage.counter);
    Serial.print("Difference ");
    Serial.println(timeOffset);
  }

}
