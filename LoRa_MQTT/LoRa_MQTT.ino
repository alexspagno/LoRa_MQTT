
#include "heltec.h"
#include "setup.h"
#include "logo_img.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
//#include <time.h>
#include <WiFiUdp.h>
#include <TimeLib.h>

#define pin_selector 13
#define pin_stato1 36
#define pin_stato2 37
#define pin_led1 33
#define pin_led2 32

// Definizione banda LoRa
#define BAND    868E6  //you can set band here directly,e.g. 868E6,915E6


WiFiClient espClient;
PubSubClient client(espClient);

// NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");


String MqttConnectionState = "";  // stringa MQTT

// WIFI
long wifi_rssi = 0;

// LoRa
byte LoraArrayTX[26];
byte LoraArrayRX[26];
byte SenderID;
byte ReceiverID;
byte MyPass[4] = {0x00,0x00,0x00,0x00};
byte LoraRssi_status;
byte LoraTotalRetry = 0;     // da vedere se possibile incrementare numero massimo 256 come valore max
byte LoraTxError = 0;     // da vedere se possibile incrementare numero massimo 256 come valore max
int LoraRetry = 0;
bool TimerRetry = false;

bool INIBIT = false;

String LoraTime = ""; ///// ???????????????????????????????' mettere a posto orario per lora

//protocollo v2.0
byte ChangeStatus = 0x00;
byte StatusIn1 =    0x00;
byte StatusIn2 =    0x00;
byte StatusOut1 =   0x00;
byte StatusOut2 =   0x00;




// DISPLAY 
String mqttStatus = "Offline"; 
int D_WifiRssiBar = 0;
int wifi_rssi_counter = 0;
String D_LoraRetry = "";
String local_ip = "";
int D_LoraRssiBar = 0;

String D_LoraRssi = "RSSI --"; // testo per display

String head_1; 
String head_2; 
String head_3;
String text_msg;
String footer_1;
String footer_2;


// NOME FILE PER DISPLAY
String ino = "";

void setup(){ 
  ino = __FILE__;
  ino = (ino.substring((ino.indexOf(".")), (ino.lastIndexOf("\\")) + 1));   
  
  pinMode(pin_selector, INPUT_PULLUP);
  pinMode(pin_stato1, INPUT_PULLUP);
  pinMode(pin_stato2, INPUT_PULLUP);
  pinMode(pin_led1, OUTPUT);
  pinMode(pin_led2, OUTPUT);
  digitalWrite(pin_led1, LOW);
  digitalWrite(pin_led2, LOW);
  if(digitalRead(pin_selector)){master = false;}else{master = true;} 
  
  heltec_init();
  Serial.println();
  Serial.println(ino);
  Serial.println();
  display_init();
  lora_init();
  if(master){
    wifi_init();
    ntp_init();
    mqtt_init();    
    lastUpdate_time = millis() - SetUpdate_time; //forzo inzio orario
  } 

  if(master){
    SenderID = 0x01;
    ReceiverID = 0x02;
  }else{
    SenderID = 0x02;
    ReceiverID = 0x01;
  }
}

void loop()
{
  if(master && INIBIT == false){
    check_wifi_connection();
    check_wifi_rssi();
    check_mqtt_connection(); 
    check_update_time();
    
  }

  if(INIBIT == false){
    if     (check_In1()){LoraSenderStage1();}
    else if(check_In2()){LoraSenderStage1();}
    else if(check_Out1()){LoraSenderStage1();}
    else if(check_Out2()){LoraSenderStage1();}
  }
   
  check_LoRa_RX();   
  check_LoRa_TX_retry();
  check_stop_display();  
  
}
