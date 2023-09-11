#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "ESP32TimerInterrupt.h"
#include "FS.h"
#include "SD_MMC.h"
#include "SPI.h"

 


#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
#define TIMER_INTERVAL_MS       1000
#define USING_TIM_DIV1 true
#define TIMER_CALIBRATION 1
int mode = TIMER_CALIBRATION;


/*
TODO: 
- safeguard that a reboot of master will somehow cause the clients to restart their timer
- 
*/

//----------
//wifi stuff
//----------
//uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//MAIN ESP32 
//uint8_t broadcastAddress[] = {0xB4,0xE6,0x2D,0xE9,0x3C,0x21};
//xiao address
//uint8_t broadcastAddress[] = {0x34,0x85,0x18,0x5,0x88,0xb0};
//CLIENT ESP32
uint8_t broadcastAddress[] = {0x7c,0x87,0xce,0x2d,0xcf,0x98};

//cam ESP
//uint8_t broadcastAddress[] = {0xec, 0x62, 0x60, 0x9b, 0xd2, 0x7c};
uint8_t myAddress[6];
esp_now_peer_info_t peerInfo;
//-------
//message types
//--------
struct timer_message {
  uint8_t messageType;
  uint16_t counter;
  uint16_t sendTime;
  uint16_t lastDelay;
} ;


timer_message timerMessage;

//-----------
//Timer config variables
//-----------
int msgSendTime;
int msgArriveTime;
int msgReceiveTime;
bool newMsgSent = false;
bool newMsgReceived = false;
int timesince;
int newMsgTime;
int lastDelay = 0;
uint32_t timerCounter = 0;



void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)){
      Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}






void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int lenn) {
  msgReceiveTime = micros();
  newMsgReceived = true;
  String out = String(timerCounter+","+msgReceiveTime);
  Serial.print ("recv ");
  Serial.println(out);
  appendFile (SD_MMC, "/rcvd.txt", out.c_str() );

}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t sendStatus) {
  if (sendStatus == ESP_NOW_SEND_SUCCESS) {
    msgArriveTime = micros();
    lastDelay = msgArriveTime-msgSendTime;
    String out = String(timerCounter+","+msgArriveTime);
    //timerCounter+","+msgArriveTime+",");
    //Serial.print("Sent ");
    //Serial.println(out);
    appendFile(SD_MMC, "/sent.txt", out.c_str());

  }
  else {
    msgArriveTime = 0;
  }
}


void setup() {
  Serial.begin(115200);
  if(!SD_MMC.begin()){
    Serial.println("Card Mount Failed");
    return;
  }

  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);

  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  /*if (ITimer.attachInterruptInterval(TIMER_INTERVAL_MS * 1000, TimerHandler))
  {
    Serial.print(F("Starting  ITimer OK, millis() = ")); 
    Serial.println(millis());
  }
  else 
    Serial.println(F("Can't set ITimer correctly. Select another freq. or interval")); 
  */
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  //esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
    // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);  
  //esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  

}

void loop() {
   timerCounter++;
  msgSendTime = micros();
  timerMessage.messageType = 0;
  timerMessage.sendTime = msgSendTime;
  timerMessage.counter = timerCounter;
  timerMessage.lastDelay = lastDelay;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &timerMessage, sizeof(timerMessage));
  delay(1000);
// analog read on analog read

}

