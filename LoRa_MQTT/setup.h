int LOG = 2; // 0 nessun LOG , 1 log necessari,  2 tutti i log

bool master = false;

// WIFI
const char *ssid = "my-ssid";
const char *password = "my-pass";

// MQTT
const char *mqtt_server = "mqtt.flespi.io";
//const char *mqtt_port = 1883;
const char *mqtt_token = "my token";




// MQTT
//Comandi ricevuti da MQTT
String MqttCmdON = "on";
String MqttCmdOFF = "off";
String MqttCmdToggle = "toggle";
String MqttCmdCheck = "check";

//Stati
String MqttStatusON = "open";
String MqttStatusOFF = "close";


//Topic MQTT
//Subcribe Master (Ricezione comandi MQTT)
const char *MqttTopicMasterCheck = "MasterCmdCheck";
const char *MqttTopicMasterCmdOut1 = "MasterCmdOut1";
const char *MqttTopicMasterCmdOut2 = "MasterCmdOut2";
//Publish Master (Invio stati MQTT)
const char *MqttTopicMasterStatusIn1 = "MasterStatusIn1";
const char *MqttTopicMasterStatusIn2 = "MasterStatusIn2";
const char *MqttTopicMasterStatusOut1 = "MasterStatusOut1";
const char *MqttTopicMasterStatusOut2 = "MasterStatusOut2";
//Publish Master (Invio stato radio MQTT)
const char *MqttTopicMasterWifiRssi = "MasterWifiRssi";
const char *MqttTopicMasterLoraRssi = "MasterLoraRssi";
const char *MqttTopicMasterLoraRetry = "MasterLoraRetry";
const char *MqttTopicMasterLoraTotalRetry = "MasterLoraTotalRetry";


//Subcribe Slave (Ricezione comandi MQTT)
const char *MqttTopicSlaveCheck = "SlaveCmdCheck";
const char *MqttTopicSlaveCmdOut1 = "SlaveCmdOut1";
const char *MqttTopicSlaveCmdOut2 = "SlaveCmdOut2";
//Publish Slave (Invio stati MQTT)
const char *MqttTopicSlaveStatusIn1 = "SlaveStatusIn1";
const char *MqttTopicSlaveStatusIn2 = "SlaveStatusIn2";
const char *MqttTopicSlaveStatusOut1 = "SlaveStatusOut1";
const char *MqttTopicSlaveStatusOut2 = "SlaveStatusOut2";
//Publish Slave (Invio stato radio MQTT)
const char *MqttTopicSlaveLoraRssi = "SlaveLoraRssi";
const char *MqttTopicSlaveLoraRetry = "SlaveLoraRetry";
const char *MqttTopicSlaveLoraTotalRetry = "SlaveLoraTotalRetry";



//DISPLAY TEXT

const String D_MasterStatusIn1_OFF =  "Master_in1 close";
const String D_MasterStatusIn1_ON =   "Master_in1 open";
const String D_MasterStatusIn2_OFF =  "Master_in2 close";
const String D_MasterStatusIn2_ON =   "Master_in2 open";

const String D_MasterStatusOut1_OFF = "Master_Out1 OFF";
const String D_MasterStatusOut1_ON =  "Master_Out1 ON";
const String D_MasterStatusOut2_OFF = "Master_Out2 OFF";
const String D_MasterStatusOut2_ON =  "Master_Out2 ON";

////??????????????????????????????????????????
const String D_MasterCommandOut1_OFF = "Master_Out1 OFF";
const String D_MasterCommandOut1_ON = "Master_Out1 ON";
const String D_MasterCommandOut2_OFF = "Master_Out2 OFF";
const String D_MasterCommandOut2_ON = "Master_Out2 ON";
//////////////////////////////////////////////

const String D_SlaveStatusIn1_OFF =  "Slave_in1 close";
const String D_SlaveStatusIn1_ON =   "Slave_in1 open";
const String D_SlaveStatusIn2_OFF =  "Slave_in2 close";
const String D_SlaveStatusIn2_ON =   "Slave_in2 open";

const String D_SlaveStatusOut1_OFF = "Slave_Out1 OFF";
const String D_SlaveStatusOut1_ON =  "Slave_Out1 ON";
const String D_SlaveStatusOut2_OFF = "Slave_Out2 OFF";
const String D_SlaveStatusOut2_ON =  "Slave_Out2 ON";

////??????????????????????????????????????????
const String D_SlaveCommandOut1_OFF = "Slave_Out1 OFF";
const String D_SlaveCommandOut1_ON = "Slave_Out1 ON";
const String D_SlaveCommandOut2_OFF = "Slave_Out2 OFF";
const String D_SlaveCommandOut2_ON = "Slave_Out2 ON";
//////////////////////////////////////////////


// Timer
long lastTimerRetry = 0;
long lastReceive = 0;
long lastCheckRssi = 0;
long lastUpdate_time = 0;
long lastUpdate_display = 0;
long lastCheckMqtt = 0;
long lastUpdate_clear_CommandState = 0;
long lastUpdateState = 0;


// Timer setup
int SetTimerRetry = 1000;       //default 1000!
int SetReceive = 10;            //default 10!
int SetCheckRssi = 1000;
int SetUpdate_time = 50000;
int SetUpdate_display = 7000;
int SetCheckMqtt = 60000;
int SetUpdate_clear_CommandState = 5000;
long SetUpdateState = 300000;     //5 minuti

int SendDelay = 0; //DEFAULT 0
int SetLedTmr = 10;
