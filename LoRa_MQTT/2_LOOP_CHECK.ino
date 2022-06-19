// ROUTINE DI CHECK ALL'INTERNO DEL LOOP PRINCIPALE
/////////////////////////////protocollo v2.0 /////////////////////////////////////////////////////
 

//-----------------------------------------------------------------------//
bool check_In1(){
  bool NewState = false;
  byte StatusTempIn1 = digitalRead(pin_stato1);
  if (StatusTempIn1 != StatusIn1){
    StatusIn1 = StatusTempIn1;
    NewState = true;
    ChangeStatus = 0x01;
    if(master){
      if (StatusIn1 == 0x00){client.publish("MasterStatusIn1", MasterStatusIn1_OFF.c_str());}
      else if (StatusIn1 == 0x01){client.publish("MasterStatusIn1", MasterStatusIn1_ON.c_str());}
    }    
  }
  return(NewState);
}

//-----------------------------------------------------------------------//
bool check_In2(){
  bool NewState = false;
  byte StatusTempIn2 = digitalRead(pin_stato2);
  if (StatusTempIn2 != StatusIn2){
    StatusIn2 = StatusTempIn2;
    NewState = true;
    ChangeStatus = 0x02;
    if(master){
      if (StatusIn2 == 0x00){client.publish("MasterStatusIn2", MasterStatusIn2_OFF.c_str());}
      else if (StatusIn2 == 0x01){client.publish("MasterStatusIn2", MasterStatusIn2_ON.c_str());}
    }  
  }
  return(NewState);
}

///////////////////////////// END protocollo v2.0 /////////////////////////////////////////////////////


//-----------------------------------------------------------------------//
void check_wifi_rssi(){  
  if (millis() - lastCheckRssi > SetCheckRssi ) {
    lastCheckRssi = millis();  
    
    wifi_rssi = WiFi.RSSI();
    wifiRssi_status = String(wifi_rssi);  // valore stringa espresso in Dbm per MQTT
    wifi_rssi = ((wifi_rssi+100)-20)*2;   // calcola valore massimo -30Db valore minimo -80Db
    String convert = String(wifi_rssi);   // valore stringa da convertitr in int
    wifi_Rssi_int = convert.toInt();      // converto in int per display    
  }
}

//-----------------------------------------------------------------------//
void check_mqtt_connection(){
  if (!client.connected()) {
    mqttStatus = "Offline";
    reconnect(); }
  client.loop();  
}

//-----------------------------------------------------------------------//
void check_update_time(){
  if (millis() - lastUpdate_time > SetUpdate_time) { 
      lastUpdate_time = millis();
      timeClient.update();
      LoraTime = timeClient.getFormattedTime();      
      //NewLoraTxTime = true;
  }
}

//-----------------------------------------------------------------------//
void check_stop_display(){
  if (millis() - lastUpdate_display > SetUpdate_display) { 
    lastUpdate_display = millis(); 
    displayCalendar();
  }
}

//-----------------------------------------------------------------------//
/* DA finire
void check_enable_after_RxStatus(){  
  if
  if (millis() - lastCheckEnableAfterRxStatus > SetCheckEnableAfterRxStatus ) {
    lastCheckMqtt = millis();  
    NewMqttCheck = true;    
  }
}
*/

//-----------------------------------------------------------------------//
void check_wifi_connection(){
  if(WiFi.status()!=WL_CONNECTED){
    wifi_Rssi_int = 0; // azzero barra wifi
    mqttStatus = "Offline";
    MqttConnectionState = "Offline";
    delay(1000);
    WiFi.disconnect();
    delay(2000);
    wifi_init();
  }
}
