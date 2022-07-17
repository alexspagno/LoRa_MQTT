/*TOPIC:
 * 
 * Subscribe:
 * Command??
 * Check??
 * ResetFail??
 * 
 * Publish:
 * MasterStatusIn1
 * MasterStatusIn2
 * MasterStatusOut1
 * MasterStatusOut2
 * MasterLoraRssi
 * MasterLoraRetry
 * MasterLoraTotalRetry
 * MasterWifiRssi * 
 * 
 * SlaveStatusIn1
 * SlaveStatusIn2
 * SlaveStatusOut1
 * SlaveStatusOut2 * 
 * SlaveLoraRssi
 * SlaveLoraRetry
 * SlaveLoraTotalRetry
 *  
 * 
 */

        





// ROUTINE MQTT

//-----------------------------------------------------------------------//
void callback(char *MqttTopic, byte *MqttBytePayload, unsigned int length) {
  Serial.println();
  Serial.print("LOG: MQTT - Ricezione Topic [");
  Serial.print(MqttTopic);
  Serial.println("] ");
  String MqttPayload;
  for (int i = 0; i < length; i++) {MqttPayload = MqttPayload + (char)MqttBytePayload[i];}
  // stringa ricevuta da mqtt
  Serial.print("LOG: MQTT - Messaggio: ");
  Serial.println(MqttPayload); 


     
   /******************************************************************************************************************************************************* 
   * COMANDI MQTT verso Master:
   * MasterStatusOut1 ON  - attivo uscita, scrivo display
   * MasterStatusOut2 ON  - attivo uscita, scrivo display
   * MasterStatusOut1 OFF - disattivo uscita, scrivo display
   * MasterStatusOut2 OFF - disattivo uscita, scrivo display
   * MasterCheck          - controllo variabili StatusInX e pubblico sui topic MasterStatusInX le stringhe corrispondenti allo stato definite in setup.h
   *                      - controllo variabili StatusOutX e pubblico sui topic MasterStatusOutX le stringhe corrispondenti allo stato definite in setup.h                    
   *******************************************************************************************************************************************************/  
    
  if      (strcmp(MqttTopic, MqttTopicMasterCmdOut1) == 0){
    if     (MqttPayload == MqttCmdON){digitalWrite(pin_led1,HIGH); text_msg = D_MasterStatusOut1_ON; displayMqttRX();}
    else if(MqttPayload == MqttCmdOFF){digitalWrite(pin_led1,LOW ); text_msg = D_MasterStatusOut1_OFF; displayMqttRX();}
    else if(MqttPayload == MqttCmdToggle){digitalWrite(pin_led1,HIGH); text_msg = D_MasterStatusOut1_ON; displayMqttRX();}  
  }  
  else if (strcmp(MqttTopic, MqttTopicMasterCmdOut2) == 0){
    if     (MqttPayload == MqttCmdON){digitalWrite(pin_led2,HIGH); text_msg = D_MasterStatusOut2_ON; displayMqttRX();}
    else if(MqttPayload == MqttCmdOFF){digitalWrite(pin_led2,LOW ); text_msg = D_MasterStatusOut2_OFF; displayMqttRX();}
    else if(MqttPayload == MqttCmdToggle){digitalWrite(pin_led2,HIGH); text_msg = D_MasterStatusOut2_ON; displayMqttRX();}
  }
  else if (strcmp(MqttTopic, MqttTopicMasterCheck) == 0){
    if     (MqttPayload == MqttCmdCheck){
      if      (StatusIn1 == 0x00){client.publish(MqttTopicMasterStatusIn1, MqttStatusOFF.c_str());}
      else if (StatusIn1 == 0x01){client.publish(MqttTopicMasterStatusIn1, MqttStatusON.c_str());}
      if      (StatusIn2 == 0x00){client.publish(MqttTopicMasterStatusIn2, MqttStatusOFF.c_str());}
      else if (StatusIn2 == 0x01){client.publish(MqttTopicMasterStatusIn2, MqttStatusON.c_str());}
      if      (StatusOut1 == 0x00){client.publish(MqttTopicMasterStatusOut1, MqttStatusOFF.c_str());}
      else if (StatusOut1 == 0x01){client.publish(MqttTopicMasterStatusOut1, MqttStatusON.c_str());}
      if      (StatusOut2 == 0x00){client.publish(MqttTopicMasterStatusOut2, MqttStatusOFF.c_str());}
      else if (StatusOut2 == 0x01){client.publish(MqttTopicMasterStatusOut2, MqttStatusON.c_str());}  
    } 
  }

   /******************************************************************************************************************************************************* 
   * COMANDI MQTT verso Slave:                     
   * SlaveStatusOut1 ON  - setto variabile ChangeStatus 0x11, scrivo display, inizio trasmissione LoRa
   * SlaveStatusOut2 ON  - setto variabile ChangeStatus 0x21, scrivo display, inizio trasmissione LoRa
   * SlaveStatusOut1 OFF - setto variabile ChangeStatus 0x10, scrivo display, inizio trasmissione LoRa
   * SlaveStatusOut2 OFF - setto variabile ChangeStatus 0x20, scrivo display, inizio trasmissione LoRa
   * SlaveCheck          - setto variabile ChangeStatus 0x99, scrivo display, inizio trasmissione LoRa   *
   *******************************************************************************************************************************************************/    

  else if (strcmp(MqttTopic, MqttTopicSlaveCmdOut1) == 0){ 
    if     (MqttPayload == MqttCmdON){ChangeStatus = 0x11, text_msg = D_SlaveStatusOut1_ON; displayMqttRX(); LoraSenderStage1();}
    else if(MqttPayload == MqttCmdOFF){ChangeStatus = 0x10, text_msg = D_SlaveStatusOut1_OFF; displayMqttRX(); LoraSenderStage1();}    
  }

  else if (strcmp(MqttTopic, MqttTopicSlaveCmdOut2) == 0){ 
    if     (MqttPayload == MqttCmdON){ChangeStatus = 0x21, text_msg = D_SlaveStatusOut2_ON; displayMqttRX(); LoraSenderStage1();}
    else if(MqttPayload == MqttCmdOFF){ChangeStatus = 0x20, text_msg = D_SlaveStatusOut2_OFF; displayMqttRX(); LoraSenderStage1();}    
  }
  else if (strcmp(MqttTopic, MqttTopicSlaveCheck) == 0){
    if     (MqttPayload == MqttCmdCheck){ChangeStatus = 0x99, text_msg = "Slave Check"; displayMqttRX(); LoraSenderStage1();}
  }
  
}


void MqttSendConnectionStatus(){

  String MasterWifiRssi = String(wifi_rssi);  // valore stringa espresso in Dbm per MQTT;  
  String MasterLoraRssi = String(LoraRssi_status);  
  String MasterLoraRetry = String(LoraTotalRetry);  
  String MasterLoraTotalRetry = String(LoraTxError);    
  String SlaveLoraRssi =  String(LoraArrayRX[16]);  
  String SlaveLoraRetry = String(LoraArrayRX[17]);  
  String SlaveLoraTotalRetry = String(LoraArrayRX[18]); 

  client.publish(MqttTopicMasterWifiRssi, MasterWifiRssi.c_str());  
  client.publish(MqttTopicMasterLoraRssi, MasterLoraRssi.c_str());  
  client.publish(MqttTopicMasterLoraRetry, MasterLoraRetry.c_str());  
  client.publish(MqttTopicMasterLoraTotalRetry, MasterLoraTotalRetry.c_str());    
  client.publish(MqttTopicSlaveLoraRssi,  SlaveLoraRssi.c_str());  
  client.publish(MqttTopicSlaveLoraRetry, SlaveLoraRetry.c_str());  
  client.publish(MqttTopicSlaveLoraTotalRetry, SlaveLoraTotalRetry.c_str());  



}








void reconnect() {
  while (!client.connected()) {
    Serial.println("LOG: MQTT - OFFLINE");
    mqttStatus = "Offline";
    MqttConnectionState = "Offline";

    head_1 = "Connessione mqtt...";  
    head_2 = "";
    head_3 = "";
    DisplayConnection();
    
    String clientId = "ESP32_garage";    
    if (client.connect(clientId.c_str(), mqtt_token, "")) {
      Serial.println("LOG: MQTT - CONNESSO");
      mqttStatus = "Online";
      timeClient.update();
      MqttConnectionState = timeClient.getFormattedTime();
      MqttConnectionState = "Connected " + MqttConnectionState; 
      client.subscribe(MqttTopicMasterCheck);
      client.subscribe(MqttTopicMasterCmdOut1);
      client.subscribe(MqttTopicMasterCmdOut2);
      client.subscribe(MqttTopicSlaveCheck);
      client.subscribe(MqttTopicSlaveCmdOut1);
      client.subscribe(MqttTopicSlaveCmdOut2);

 

      head_1 = "Connessione mqtt...";  
      head_2 = "connected";
      head_3 = "";
      DisplayConnection();
      
    } else {
      Serial.println("LOG: MQTT - Connessione FALLITA");
      mqttStatus = "Offline";
      MqttConnectionState = "Offline";

      head_1 = "Connessione mqtt...";  
      head_2 = "Fallita, rc=" + client.state();
      head_3 = "WIFI check:...";
      DisplayConnection();

      // attendo 2 secondi poi controllo WIFI
      delay(2000); 
      check_wifi_connection();
      
      // attendo 5 secondi poi riprovo a connettere MQTT
      delay(5000);
    }
  }
}
