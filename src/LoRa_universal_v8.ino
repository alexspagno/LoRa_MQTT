
#include "heltec.h"
#include "logo_img.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <time.h>
#include <WiFiUdp.h>
#include <TimeLib.h>

#define pin_selector 13
#define pin_stato1 36
#define pin_stato2 37
#define pin_led1 33
#define pin_led2 32

int LOG = 2; // 0 nessun LOG , 1 log necessari,  2 tutti i log

// Definizione banda LoRa
#define BAND    868E6  //you can set band here directly,e.g. 868E6,915E6


WiFiClient espClient;
PubSubClient client(espClient);

// NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");



bool master = false;

// WIFI
const char* ssid = "dd-wrt";
const char* password = "!buffalo!";

//const char* ssid = "Vodafone-teleprojects";
//const char* password = "!buffalo!";

//const char* ssid = "Android";
//const char* password = "gqxh8615";
long wifi_rssi = 0;


// MQTT
const char* mqtt_server = "mqtt.flespi.io";
//const char* mqtt_port = 1883;
const char* mqtt_token = "m8GyB28pnDRL6nXXJXHZDB8dGLQjyUbLTH7ix6vIzdhto5eXQMNjbJkHi0QHLPgs";
String MqttConnectionState = "";  // stringa MQTT
String Lora_fail_status = "";     // stringa MQTT
//String Lora_rssi_status = "";   // stringa MQTT è rssi
String CommandState = "";         // stringa MQTT
String wifiRssi_status = "";      // stringa MQTT



// LoRa
String rssi = "RSSI --";
String packSize = "--";
String LoraRssiAck = "";
String LoraTime = "";

//protocollo v2.0
byte ChangeStatus = 0x00;
byte StatusIn1 =    0x00;
byte StatusIn2 =    0x00;
byte StatusOut1 =   0x00;
byte StatusOut2 =   0x00;

/*
byte ChangeStatusRX = 0x00; 
byte StatusIn1RX =    0x00;
byte StatusIn2RX =    0x00;
byte StatusOut1RX =   0x00;
byte StatusOut2RX =   0x00;
*/     

String MasterStatusIn1_OFF =  "M.Door1 close";
String MasterStatusIn1_ON =   "M.Door1 open";
String MasterStatusIn2_OFF =  "M.Door2 close";
String MasterStatusIn2_ON =   "M.Door2 open";

String MasterStatusOut1_OFF = "M.Light1 OFF";
String MasterStatusOut1_ON =  "M.Light1 ON";
String MasterStatusOut2_OFF = "M.Light2 OFF";
String MasterStatusOut2_ON =  "M.Light2 ON";

String MasterCommandOut1_OFF = "M.cmd L1 OFF";
String MasterCommandOut1_ON =  "M.cmd L1 ON";
String MasterCommandOut2_OFF = "M.cmd L2 OFF";
String MasterCommandOut2_ON =  "M.cmd L2 ON";

String SlaveStatusIn1_OFF =  "S.Door1 close";
String SlaveStatusIn1_ON =   "S.Door1 open";
String SlaveStatusIn2_OFF =  "S.Door2 close";
String SlaveStatusIn2_ON =   "S.Door2 open";

String SlaveStatusOut1_OFF = "S.Light1 OFF";
String SlaveStatusOut1_ON =  "S.Light1 ON";
String SlaveStatusOut2_OFF = "S.Light2 OFF";
String SlaveStatusOut2_ON =  "S.Light2 ON";

String SlaveCommandOut1_OFF = "S.cmd L1 OFF";
String SlaveCommandOut1_ON =  "S.cmd L1 ON";
String SlaveCommandOut2_OFF = "S.cmd L2 OFF";
String SlaveCommandOut2_ON =  "S.cmd L2 ON";


byte LoraArrayTX[20];
byte LoraArrayRX[20];

byte SenderID;
byte ReceiverID;
byte MyPass[4] = {0x00,0x00,0x00,0x00};

int ret = 0;
bool TimerRetry = false;

bool INIBIT = false;






// DISPLAY 
String mqttStatus = "Offline"; 
int wifi_Rssi_int = 0;
int wifi_rssi_counter = 0;
String str_ret = "";
String local_ip = "";
int lora_Rssi_int = 0;

//information
String head_1; 
String head_2; 
String head_3;
String text_msg;
String footer_1;
String footer_2;

// Timer
long lastUpdate_ack = 0;
long lastReceive = 0;
long lastCheckRssi = 0;
long lastUpdate_time = 0;
long lastUpdate_display = 0;
long lastCheckMqtt = 0;
long lastUpdate_clear_CommandState = 0;
long lastUpdateState = 0;

// Timer setup
int SetUpdate_ack = 1000;       //default 1000!
int SetReceive = 10;            //default 10!
int SetCheckRssi = 1000;
int SetUpdate_time = 50000;
int SetUpdate_display = 7000;
int SetCheckMqtt = 60000;
int SetUpdate_clear_CommandState = 5000;
long SetUpdateState = 300000;     //5 minuti

int SendDelay = 0; //DEFAULT 0
int SetLedTmr = 10;


int retry = 0;
int error = 0;

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
    if(check_In1()){LoraSenderStage1();}
    else if(check_In2()){LoraSenderStage1();}
  }

  if(!master && INIBIT == false){
    if(check_In1()){LoraSenderStage1();}
    else if(check_In2()){LoraSenderStage1();}
  }
   
  check_LoRa_RX();   
  check_LoRa_TX_retry();
  check_stop_display();  
  
}
