// ROUTINE DI CHECK ALL'INTERNO DEL LOOP PRINCIPALE
/////////////////////////////protocollo v2.0 /////////////////////////////////////////////////////
 
//-----------------------------------------------------------------------//
void check_LoRa_RX(){  
  if (millis() - lastReceive > SetReceive) {    
    lastReceive = millis();
    
    int LoraMsgRX_Size = LoRa.parsePacket();
    if (LoraMsgRX_Size) { // ci sono nuove ricezioni Lora??
      for (int i = 0; i < LoraMsgRX_Size; i++) { 
        LoraArrayRX[i] = LoRa.read();
      } 
      D_LoraRssi = String(LoRa.packetRssi(), DEC) ; //converto RssiLora per testo display
      int rssi_int = D_LoraRssi.toInt();
      D_LoraRssiBar = (rssi_int+130)*1.25;  // converto RssiLora per progressBar display
      LoraRssi_status = rssi_int*-1;        // converto RssiLora in positivo per metterlo in array di trasmissione
      
      digitalWrite(LED, HIGH);   // turn the LED on 
      delay(SetLedTmr);
      digitalWrite(LED, LOW);   // turn the LED on 
      
            
      if      (LoraArrayRX[0] == 0x01){LoraReceiverStage1();}
      else if (LoraArrayRX[0] == 0x02){LoraSenderStage2();}
      else if (LoraArrayRX[0] == 0x03){LoraReceiverStage2();}
      else if (LoraArrayRX[0] == 0x04){LoraSenderStage3();} 

    }
  }
}   
      


//-----------------------------------------------------------------------//
void check_LoRa_TX_retry(){
  if (TimerRetry){
    if (millis() - lastTimerRetry > SetTimerRetry) { 
      lastTimerRetry = millis();
      if(LOG>1){Serial.println("LOG: LoRa - retry");}  
      LoraTotalRetry++;    
      if (LoraRetry < 3){        
        LoRa_TX(LoraArrayTX, sizeof(LoraArrayTX)); //spedisco LoraArrayTX via LoRa 
         
        LoraRetry++;
        D_LoraRetry = "RET: " + String(LoraRetry);
        footer_2 = D_LoraRetry;
        Display();
      }
      else{ 
        if(LOG>0){Serial.println("LOG: LoRa - errore ricezione");}
        LoraTxError++; 
        LoraRetry = 0;
        TimerRetry = false;
        ChangeStatus = 0x00;                               // azzero il flag di cambio stato
        INIBIT = false; 
        
        head_1 = "ERROR:";  
        head_2 = "";
        text_msg = footer_1;
        footer_1 = "";
        footer_2 = "";
        Display();     
               
        for(int i=0;i<10;i++){
          digitalWrite(LED, HIGH);   // turn the LED on 
          delay(50);
          digitalWrite(LED, LOW);   // turn the LED on 
          delay(50);           
        }        
      }
    }
  }
}

//-----------------------------------------------------------------------//
bool check_In1(){
  bool NewState = false;
  byte StatusTempIn1 = digitalRead(pin_stato1);
  if (StatusTempIn1 != StatusIn1){
    StatusIn1 = StatusTempIn1;
    NewState = true;
    ChangeStatus = 0x01;
    if(master){
      if (StatusIn1 == 0x00){client.publish(MqttTopicMasterStatusIn1, MqttStatusOFF.c_str());}
      else if (StatusIn1 == 0x01){client.publish(MqttTopicMasterStatusIn1, MqttStatusON.c_str());}
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
      if (StatusIn2 == 0x00){client.publish(MqttTopicMasterStatusIn2, MqttStatusOFF.c_str());}
      else if (StatusIn2 == 0x01){client.publish(MqttTopicMasterStatusIn2, MqttStatusON.c_str());}
    }  
  }
  return(NewState);
}


//-----------------------------------------------------------------------//
bool check_Out1(){
  bool NewState = false;
  byte StatusTempOut1 = digitalRead(pin_led1);
  if (StatusTempOut1 != StatusOut1){
    StatusOut1 = StatusTempOut1;
    NewState = true;
    ChangeStatus = 0x03;
    if(master){
      if (StatusOut1 == 0x00){client.publish(MqttTopicMasterStatusOut1, MqttStatusOFF.c_str());}
      else if (StatusOut1 == 0x01){client.publish(MqttTopicMasterStatusOut1, MqttStatusON.c_str());}
    }    
  }
  return(NewState);
}

//-----------------------------------------------------------------------//
bool check_Out2(){
  bool NewState = false;
  byte StatusTempOut2 = digitalRead(pin_led2);
  if (StatusTempOut2 != StatusOut2){
    StatusOut2 = StatusTempOut2;
    NewState = true;
    ChangeStatus = 0x04;
    if(master){
      if (StatusOut2 == 0x00){client.publish(MqttTopicMasterStatusOut2, MqttStatusOFF.c_str());}
      else if (StatusOut2 == 0x01){client.publish(MqttTopicMasterStatusOut2, MqttStatusON.c_str());}
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
    wifi_rssi = ((wifi_rssi+100)-20)*2;   // calcola valore massimo -30Db valore minimo -80Db
    String convert = String(wifi_rssi);   // valore stringa da convertitr in int
    D_WifiRssiBar = convert.toInt();      // converto in int per display    
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
    D_WifiRssiBar = 0; // azzero barra wifi
    mqttStatus = "Offline";
    MqttConnectionState = "Offline";
    delay(1000);
    WiFi.disconnect();
    delay(2000);
    wifi_init();
  }
}
