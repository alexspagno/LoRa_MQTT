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
 * MasterLoraErr
 * MasterLoraFail
 * MasterWifiRssi * 
 * 
 * SlaveStatusIn1
 * SlaveStatusIn2
 * SlaveStatusOut1
 * SlaveStatusOut2 * 
 * SlaveLoraRssi
 * SlaveLoraErr
 * SlaveLoraFail
 * 
 * 
 */

        





// ROUTINE MQTT

//-----------------------------------------------------------------------//
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println();
  Serial.print("LOG: MQTT - Ricezione Topic [");
  Serial.print(topic);
  Serial.println("] ");
  String str_payload;
  for (int i = 0; i < length; i++) {
    str_payload = str_payload + (char)payload[i];    
  }
  // stringa ricevuta da mqtt
  Serial.print("LOG: MQTT - Messaggio: ");
  Serial.println(str_payload); 
  
  // Se ricevo un comando master attivo uscita
  if     (str_payload == "MasterStatusOut1 ON"){digitalWrite(pin_led1, HIGH); StatusOut1 = 0x01; text_msg = MasterStatusOut1_ON; displayMqttRX();}
  else if(str_payload == "MasterStatusOut2 ON"){digitalWrite(pin_led2, HIGH); StatusOut2 = 0x01; text_msg = MasterStatusOut2_ON; displayMqttRX();}  
  else if(str_payload == "MasterStatusOut1 OFF"){digitalWrite(pin_led1, LOW); StatusOut1 = 0x00; text_msg = MasterStatusOut1_OFF; displayMqttRX();}
  else if(str_payload == "MasterStatusOut2 OFF"){digitalWrite(pin_led2, LOW); StatusOut2 = 0x00; text_msg = MasterStatusOut2_OFF; displayMqttRX();} 
  else if(str_payload == "MasterCheck"){
    if      (StatusIn1 == 0x00){client.publish("MasterStatusIn1", MasterStatusIn1_OFF.c_str());}
    else if (StatusIn1 == 0x01){client.publish("MasterStatusIn1", MasterStatusIn1_ON.c_str());}
    if      (StatusIn2 == 0x00){client.publish("MasterStatusIn2", MasterStatusIn2_OFF.c_str());}
    else if (StatusIn2 == 0x01){client.publish("MasterStatusIn2", MasterStatusIn2_ON.c_str());}
    if      (StatusOut1 == 0x00){client.publish("MasterStatusOut1", MasterStatusOut1_OFF.c_str());}
    else if (StatusOut1 == 0x01){client.publish("MasterStatusOut1", MasterStatusOut1_ON.c_str());}
    if      (StatusOut2 == 0x00){client.publish("MasterStatusOut2", MasterStatusOut2_OFF.c_str());}
    else if (StatusOut2 == 0x01){client.publish("MasterStatusOut2", MasterStatusOut2_ON.c_str());}
  }  
  // Se ricevo un comando Slave spedisco LoRa 
  else if(str_payload == "SlaveStatusOut1 ON") {ChangeStatus = 0x11, text_msg = SlaveStatusOut1_ON; displayMqttRX(); LoraSenderStage1();}
  else if(str_payload == "SlaveStatusOut2 ON") {ChangeStatus = 0x21, text_msg = SlaveStatusOut2_ON; displayMqttRX(); LoraSenderStage1();}
  else if(str_payload == "SlaveStatusOut1 OFF"){ChangeStatus = 0x10, text_msg = SlaveStatusOut1_OFF; displayMqttRX(); LoraSenderStage1();}
  else if(str_payload == "SlaveStatusOut2 OFF"){ChangeStatus = 0x20, text_msg = SlaveStatusOut2_OFF; displayMqttRX(); LoraSenderStage1();}
  else if(str_payload == "SlaveCheck"){ChangeStatus = 0x99, text_msg = "Slave Check"; displayMqttRX(); LoraSenderStage1();}
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
      client.subscribe("Command");
      client.subscribe("Check");
      client.subscribe("ResetFail");

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
