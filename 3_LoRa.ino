
/////////////////////////////protocollo v2.3 /////////////////////////////////////////////////////
/*
  LoraArrayTX[0] =  // Stage comunicazione  0x01 - 0x02 - 0x03 - 0x04          
  LoraArrayTX[1] =  // SenderID;
  LoraArrayTX[2] =  // ReceiverID;
  LoraArrayTX[3] =  // key0 generata da RandomKey();
  LoraArrayTX[4] =  // key1 generata da RandomKey();
  LoraArrayTX[5] =  // key2 generata da RandomKey();
  LoraArrayTX[6] =  // key3 generata da RandomKey();
  LoraArrayTX[7] =  // pass0 generata da CalcolaPass(LoraArrayRX);
  LoraArrayTX[8] =  // pass1 generata da CalcolaPass(LoraArrayRX);
  LoraArrayTX[9] =  // pass2 generata da CalcolaPass(LoraArrayRX);
  LoraArrayTX[10] = // pass3 generata da CalcolaPass(LoraArrayRX);
  LoraArrayTX[11] = // change;
  LoraArrayTX[12] = // StatusIn1; 
  LoraArrayTX[13] = // StatusIn2; 
  LoraArrayTX[14] = // StatusOut1;
  LoraArrayTX[15] = // StatusOut2;
  LoraArrayTX[16] = // LoraRssi 
  LoraArrayTX[17] = // nErr
  LoraArrayTX[18] = // nFail  
  LoraArrayTX[19] = // crc8 byte messaggio generato da CRC8(LoraArrayTX , 19 ); 
*/

//-----------------------------------------------------------------------//
/*  Controllo se ho ricevuto nuovi pacchetti LoRa
 *  Se ci sono nuovi messaggi compongo array LoraArrayRX[] 
 *  per capire a che punto sono dello scambio informazioni analizzo LoraArrayRX[0] per lanciare la routine corretta
 */
void check_LoRa_RX(){  
  if (millis() - lastReceive > SetReceive) {    
    lastReceive = millis();
    
    int LoraMsgRX_Size = LoRa.parsePacket();
    if (LoraMsgRX_Size) { // ci sono nuove ricezioni Lora??
      ComponiLoraArrayRX(LoraMsgRX_Size);
            
      if      (LoraArrayRX[0] == 0x01){LoraReceiverStage1();}
      else if (LoraArrayRX[0] == 0x02){LoraSenderStage2();}
      else if (LoraArrayRX[0] == 0x03){LoraReceiverStage2();}
      else if (LoraArrayRX[0] == 0x04){LoraSenderStage3();} 

    }
  }
}   


//-----------------------------------------------------------------------//
/*  Compongo array LoraArrayRX[] dal pacchetto LoRa ricevuto
 *   Caratterizzo le variabili globali:
 *    - rssi Stringa valore rssi visualizzato a display
 *    - LoraRssiAck ???? NON UTLIZZATO
 *    - lora_Rssi_int Intero necessario alla visualizzazione su display
 */
void ComponiLoraArrayRX(int LoraMsgRX_Size) {
  for (int i = 0; i < LoraMsgRX_Size; i++) { 
      LoraArrayRX[i] = LoRa.read();
  } 
  rssi = String(LoRa.packetRssi(), DEC) ;
  LoraRssiAck = "RSSI: " + rssi;    
  lora_Rssi_int = rssi.toInt();
  lora_Rssi_int = (lora_Rssi_int+130)*1.25;       
  digitalWrite(LED, HIGH);   // turn the LED on 
  delay(SetLedTmr);
  digitalWrite(LED, LOW);   // turn the LED on 
  
}  

//-----------------------------------------------------------------------//
void check_LoRa_TX_retry(){
  if (TimerRetry){
    if (millis() - lastUpdate_ack > SetUpdate_ack) { 
      lastUpdate_ack = millis();
      if(LOG>1){Serial.println("LOG: LoRa - retry");}  
      retry++;    
      if (ret < 3){        
        LoRa_TX(LoraArrayTX, sizeof(LoraArrayTX)); //spedisco LoraArrayTX via LoRa 
         
        ret++;
        str_ret = "RET: " + String(ret);
        footer_2 = str_ret;
        Display();
      }
      else{ 
        if(LOG>0){Serial.println("LOG: LoRa - errore ricezione");}
        error++; 
        ret = 0;
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


//------------------------- TRASMISSIONE: RequestToSend #SENDER#----------------------------------------------//
void LoraSenderStage1(){  
  if(LOG>0){Serial.println("");Serial.print("LOG: LoRa - Trasmetto: ");Serial.println("RequestToSend");}
  INIBIT = true;                                      //inizio comunicazione, inibizione funzioni secondarie 
   
  memset(LoraArrayTX, 0, sizeof(LoraArrayTX));        //Inizializzo LoraArrayTX[]    
  LoraArrayTX[0] = 0x01;                              //byte[0] LoraSenderStage1 (RequestToSend)
  LoraArrayTX[1] = SenderID;                          //byte[1] SenderID 
  LoraArrayTX[2] = ReceiverID;                        //byte[2] ReceiverID
  RandomKey();                                        //byte[3][4][5][6] genero Key Random 
  ////////////////////////////                        //byte[7][8][9][10] prima trasmissione NON ho Pass da trasmettere
  ////////////////////////////                        //byte[11][12][13][14][15][16][17][18] 0x00 non utilizzati in questo stage                
  LoraArrayTX[19] = CRC8(LoraArrayTX , 19 );          //crc8 LoraArrayTX
  
  if(LOG>1){PrintTxPacket();}                         //Print contenuto LoraArrayTX

  CalcolaMyPass(LoraArrayTX);                         //calcolo myPass locale per confronto successivo
  LoRa_TX(LoraArrayTX, sizeof(LoraArrayTX));   //spedisco LoraArrayTX via LoRa  
  displaySenderTxStage1();                            //aggiorno display
}

//------------------------- RICEZIONE: RequestToSend -> INVIO: RequestAck #RECEIVER#----------------------------------------------//
void LoraReceiverStage1(){  
  if(LOG>0){Serial.println("");Serial.println("LOG: LoRa - Ricevuto RequestToSend: ");}
  if(LOG>1){PrintRxPacket();}
    
  if(LoraArrayRX[19] == CRC8(LoraArrayRX , 19 )){       //controllo crc8 ricezione
    if(LOG>0){Serial.println("LOG: LoRa - crc8 OK");}
    TimerRetry = false;
    INIBIT = true;                                      //inizio comunicazione, inibizione funzioni secondarie    
    displayReceiverRxStage1();                          //aggiorno display
        
    if(LOG>0){Serial.println("");Serial.print("LOG: LoRa - Trasmetto: ");Serial.println("RequestAck");}

    memset(LoraArrayTX, 0, sizeof(LoraArrayTX));        //Inizializzo LoraArrayTX[]
    LoraArrayTX[0] = 0x02;                              //byte[0] LoraReceiverStage1 (RequestAck)
    LoraArrayTX[1] = SenderID;                          //byte[1] SenderID 
    LoraArrayTX[2] = ReceiverID;                        //byte[2] ReceiverID
    RandomKey();                                        //byte[3][4][5][6] genero Key Random 
    CalcolaPass(LoraArrayRX);                           //byte[7][8][9][10]calcolo pass in funzione della key ricevuta in precedenza
    ////////////////////////////                        //byte[11][12][13][14][15][16][17][18] 0x00 non utilizzati in questo stage                
    LoraArrayTX[19] = CRC8(LoraArrayTX , 19 );          //crc8 LoraArrayTX

    if(LOG>1){PrintTxPacket();}                         //Print contenuto LoraArrayTX

    CalcolaMyPass(LoraArrayTX);                         //calcolo myPass locale per confronto successivo
    LoRa_TX(LoraArrayTX, sizeof(LoraArrayTX));   //spedisco LoraArrayTX via LoRa  
    displayReceiverTxStage1();                          //aggiorno display      
  }
}
   
//------------------------- RICEZIONE: RequestAck -> INVIO: Status #SENDER#----------------------------------------------//
void LoraSenderStage2(){ 
  if(LOG>0){Serial.println("");Serial.println("LOG: LoRa - Ricevuto RequestAck: ");}
  if(LOG>1){PrintRxPacket();}     
    
  if(LoraArrayRX[19] == CRC8(LoraArrayRX , 19 )){       //controllo crc8 ricezione
    if(LOG>0){Serial.println("LOG: LoRa - crc8 OK");}
    if(CheckPass()){                                    //controllo corrispondenza myPass e Pass ricevuta
      if(LOG>0){Serial.println("LOG: LoRa - Pass OK");}
      TimerRetry = false;
      displaySenderRxStage2();                          //aggiorno display
          
      if(LOG>0){Serial.println("");Serial.print("LOG: LoRa - Trasmetto:");Serial.println(" Status");}
  
      LoraArrayTX[0] = 0x03;                            //byte[0] LoraSenderStage2 (Status)
      LoraArrayTX[1] = SenderID;                        //byte[1] SenderID 
      LoraArrayTX[2] = ReceiverID;                      //byte[2] ReceiverID
      RandomKey();                                      //byte[3][4][5][6] genero Key Random 
      CalcolaPass(LoraArrayRX);                         //byte[7][8][9][10]calcolo pass in funzione della key ricevuta in precedenza
      LoraArrayTX[11] = ChangeStatus;                   //byte[11] ChangeStatus indica quale stato è variato in caso di 0x00 è un check
      LoraArrayTX[12] = StatusIn1;                      //byte[12] StatusIn1
      LoraArrayTX[13] = StatusIn2;                      //byte[13] StatusIn2
      LoraArrayTX[14] = StatusOut1;                     //byte[14] StatusOut1
      LoraArrayTX[15] = StatusOut2;                     //byte[15] StatusOut2
      LoraArrayTX[16] = 0x00;                           //LoraRssi 
      LoraArrayTX[17] = 0x00;                           //nErr
      LoraArrayTX[18] = 0x00;                           //nFail  
      LoraArrayTX[19] = CRC8(LoraArrayTX , 19 );        //crc8 LoraArrayTX
  
      if(LOG>1){PrintTxPacket();}                       //Print contenuto LoraArrayTX
  
      CalcolaMyPass(LoraArrayTX);                       //calcolo myPass locale per confronto successivo
      LoRa_TX(LoraArrayTX, sizeof(LoraArrayTX)); //spedisco LoraArrayTX via LoRa 

      if(master){
        switch (ChangeStatus){
          case 0x99: text_msg = "CHECK"; break;          
          case 0x01: if(StatusIn1) {text_msg = MasterStatusIn1_ON;}  else {text_msg = MasterStatusIn1_OFF;}  break;
          case 0x02: if(StatusIn2) {text_msg = MasterStatusIn2_ON;}  else {text_msg = MasterStatusIn2_OFF;}  break;
          case 0x03: if(StatusOut1){text_msg = MasterStatusOut1_ON;} else {text_msg = MasterStatusOut1_OFF;} break;
          case 0x04: if(StatusOut2){text_msg = MasterStatusOut2_ON;} else {text_msg = MasterStatusOut2_OFF;} break;
          case 0x10: text_msg = MasterCommandOut1_OFF; break;
          case 0x11: text_msg = MasterCommandOut1_ON; break;
          case 0x20: text_msg = MasterCommandOut2_OFF; break;
          case 0x21: text_msg = MasterCommandOut2_ON; break;
          default:   text_msg = "ChangeStatus??"; break;
        }
      }
      else{
        switch (ChangeStatus){
          case 0x99: text_msg = "CHECK"; break;        
          case 0x01: if(StatusIn1) {text_msg = SlaveStatusIn1_ON;}  else {text_msg = SlaveStatusIn1_OFF;}  break;
          case 0x02: if(StatusIn2) {text_msg = SlaveStatusIn2_ON;}  else {text_msg = SlaveStatusIn2_OFF;}  break;
          case 0x03: if(StatusOut1){text_msg = SlaveStatusOut1_ON;} else {text_msg = SlaveStatusOut1_OFF;} break;
          case 0x04: if(StatusOut2){text_msg = SlaveStatusOut2_ON;} else {text_msg = SlaveStatusOut2_OFF;} break;
          case 0x10: text_msg = SlaveCommandOut1_OFF; break;
          case 0x11: text_msg = SlaveCommandOut1_ON; break;
          case 0x20: text_msg = SlaveCommandOut2_OFF; break;
          case 0x21: text_msg = SlaveCommandOut2_ON; break;
          default:   text_msg = "ChangeStatus??"; break;
        }
      } 
      displaySenderTxStage2();                          //aggiorno display 
    }    
  }  
}

//------------------------- RICEZIONE: Status -> INVIO: StatusAck #RECEIVER#----------------------------------------------//
void LoraReceiverStage2(){ 
  if(LOG>0){Serial.println("");Serial.println("LOG: LoRa - Ricevuto Status: ");}
  if(LOG>1){PrintRxPacket();}
  
  if(LoraArrayRX[19] == CRC8(LoraArrayRX , 19 )){       //controllo crc8 ricezione
    if(LOG>0){Serial.println("LOG: LoRa - crc8 OK");}
    if(CheckPass()){                                    //controllo corrispondenza myPass e Pass ricevuta
      if(LOG>0){Serial.println("LOG: LoRa - Pass OK");}
      TimerRetry = false;


      elabora_RX_LoRa();                                // elaboro ricezione, compilo campi display, TX mqtt, abilitazione uscite      
      displayReceiverRxStage2();                        // aggiorno display
      
      if(LOG>0){Serial.println("");Serial.print("LOG: LoRa - Trasmetto:");Serial.println(" StatusAck");}
  
      LoraArrayTX[0] = 0x04;                            //byte[0] LoraSenderStage2 (Status)
      LoraArrayTX[1] = SenderID;                        //byte[1] SenderID 
      LoraArrayTX[2] = ReceiverID;                      //byte[2] ReceiverID
      RandomKey();                                      //byte[3][4][5][6] genero Key Random // anche se non utilizzata
      CalcolaPass(LoraArrayRX);                         //byte[7][8][9][10]calcolo pass in funzione della key ricevuta in precedenza
      ////////////////////////////                      //byte[11][12][13][14][15] 0x00 non utilizzati in questo stage     
      LoraArrayTX[16] = 0x00;                           //LoraRssi 
      LoraArrayTX[17] = 0x00;                           //nErr
      LoraArrayTX[18] = 0x00;                           //nFail  
      LoraArrayTX[19] = CRC8(LoraArrayTX , 19 );        //crc8 LoraArrayTX

      if(LOG>1){PrintTxPacket();}                       //Print contenuto LoraArrayTX

      LoRa_TX(LoraArrayTX, sizeof(LoraArrayTX)); //spedisco LoraArrayTX via LoRa  
      displayReceiverTxStage2();                        //aggiorno display
      TimerRetry = false; 
      ChangeStatus = 0x00;                               // azzero il flag di cambio stato
      INIBIT = false;                                   //fine comunicazione, abilito funzioni secondarie 
    }  
  }
}
    
//------------------------- RICEZIONE: StatusAck #SENDER#----------------------------------------------//
void LoraSenderStage3(){ 
  if(LOG>0){Serial.println("");Serial.println("LOG: LoRa - Ricevuto Status: ");}
  if(LOG>1){PrintRxPacket();}
  
  if(LoraArrayRX[19] == CRC8(LoraArrayRX , 19 )){       //controllo crc8 ricezione
    if(LOG>0){Serial.println("LOG: LoRa - crc8 OK");}
    if(CheckPass()){                                    //controllo corrispondenza myPass e Pass ricevuta
      if(LOG>0){Serial.println("LOG: LoRa - Pass OK");}
      TimerRetry = false;
      displaySenderTxStage3();                          //aggiorno display 

      ChangeStatus = 0x00;                              // azzero il flag di cambio stato      
      INIBIT = false;                                   //fine comunicazione, abilito funzioni secondarie 
    }  
  }
} 


//############################ ROUTINE DI APPOGGIO ###################################################################

//-----------------------------------------------------------------------//
void elabora_RX_LoRa(){
  // prelevo valori:
  byte ChangeStatusRX = LoraArrayRX[11]; 
  byte StatusIn1RX =    LoraArrayRX[12];
  byte StatusIn2RX =    LoraArrayRX[13];
  byte StatusOut1RX =   LoraArrayRX[14];
  byte StatusOut2RX =   LoraArrayRX[15];  
  
  if(master){     //Se sono Master ricevo stati da slave, li visualizzo a display e li trasmetto via MQTT. I comandi li ricevo solo MQTT non da Slave!
    switch (ChangeStatusRX){
      case 0x99: 
        text_msg = "CHECK"; // sopravvivenza aggiorno MQTT
        if      (StatusIn1RX == 0x00){client.publish("SlaveStatusIn1", SlaveStatusIn1_OFF.c_str());}
        else if (StatusIn1RX == 0x01){client.publish("SlaveStatusIn1", SlaveStatusIn1_ON.c_str());}
        if      (StatusIn2RX == 0x00){client.publish("SlaveStatusIn2", SlaveStatusIn2_OFF.c_str());}
        else if (StatusIn2RX == 0x01){client.publish("SlaveStatusIn2", SlaveStatusIn2_ON.c_str());}
        if      (StatusOut1RX == 0x00){client.publish("SlaveStatusOut1", SlaveStatusOut1_OFF.c_str());}
        else if (StatusOut1RX == 0x01){client.publish("SlaveStatusOut1", SlaveStatusOut1_ON.c_str());}
        if      (StatusOut2RX == 0x00){client.publish("SlaveStatusOut2", SlaveStatusOut2_OFF.c_str());}
        else if (StatusOut2RX == 0x01){client.publish("SlaveStatusOut2", SlaveStatusOut2_ON.c_str());}
        break;
      
      case 0x01: 
        if(StatusIn1RX) {client.publish("SlaveStatusIn1", SlaveStatusIn1_ON.c_str());  text_msg = SlaveStatusIn1_ON;}  
        else            {client.publish("SlaveStatusIn1", SlaveStatusIn1_OFF.c_str()); text_msg = SlaveStatusIn1_OFF;}  
        break;
        
      case 0x02: 
        if(StatusIn2RX) {client.publish("SlaveStatusIn2", SlaveStatusIn2_ON.c_str());  text_msg = SlaveStatusIn2_ON;}  
        else            {client.publish("SlaveStatusIn2", SlaveStatusIn2_OFF.c_str()); text_msg = SlaveStatusIn2_OFF;}  
        break;  

      case 0x03: 
        if(StatusOut1RX){client.publish("SlaveStatusOut1", SlaveStatusOut1_ON.c_str());  text_msg = SlaveStatusOut1_ON;}  
        else            {client.publish("SlaveStatusOut1", SlaveStatusOut1_OFF.c_str()); text_msg = SlaveStatusOut1_OFF;}  
        break;  

      case 0x04: 
        if(StatusOut2RX){client.publish("SlaveStatusOut2", SlaveStatusOut2_ON.c_str());  text_msg = SlaveStatusOut2_ON;}  
        else            {client.publish("SlaveStatusOut2", SlaveStatusOut2_OFF.c_str()); text_msg = SlaveStatusOut2_OFF;}  
        break;  
      
      default: text_msg = "ChangeStatus??"; break;
    }
  }
  else{     //Se sono Slave ricevo stati da Master li visualizzo a display, se sono comandi attivo le uscite.
    switch (ChangeStatusRX){
      case 0x99: text_msg = "CHECK"; break; /// implementare richiesta di invio check verso MASTER         
      case 0x01: if(StatusIn1RX) {text_msg = MasterStatusIn1_ON;}  else  {text_msg = MasterStatusIn1_OFF;}  break;
      case 0x02: if(StatusIn2RX) {text_msg = MasterStatusIn2_ON;}  else  {text_msg = MasterStatusIn2_OFF;}  break;
      case 0x03: if(StatusOut1RX){text_msg = MasterStatusOut1_ON;} else  {text_msg = MasterStatusOut1_OFF;} break;
      case 0x04: if(StatusOut2RX){text_msg = MasterStatusOut2_ON;} else  {text_msg = MasterStatusOut2_OFF;} break;
      case 0x10: text_msg = SlaveCommandOut1_OFF; digitalWrite(pin_led1, LOW);  StatusOut1 = 0x00; break;
      case 0x11: text_msg = SlaveCommandOut1_ON;  digitalWrite(pin_led1, HIGH); StatusOut1 = 0x01; break;
      case 0x20: text_msg = SlaveCommandOut2_OFF; digitalWrite(pin_led2, LOW);  StatusOut2 = 0x00; break;
      case 0x21: text_msg = SlaveCommandOut2_ON;  digitalWrite(pin_led2, HIGH); StatusOut2 = 0x01; break;
      default: text_msg = "ChangeStatus??"; break;
    }
  }
}


//-----------------------------------------------------------------------//
void RandomKey(){
  LoraArrayTX[3] = random(0xff); 
  LoraArrayTX[4] = random(0xff); 
  LoraArrayTX[5] = random(0xff); 
  LoraArrayTX[6] = random(0xff); 
}

//-----------------------------------------------------------------------//
void CalcolaMyPass(byte *key){  
  for(int i=3; i < 7; i++){
    MyPass[i-3]= CRC8( key , i-2 );    
  }    
}

//-----------------------------------------------------------------------//
void CalcolaPass(byte *key){  
  for(int i=3; i < 7; i++){                //Key è compresa tra i byte 3 e 6 di LoraArrayRX
    LoraArrayTX[i+4]= CRC8( key , i-2 );   // carico la Pass che è compresa tra i byte 7 e 10 di LoraArrayTX    
  }    
} 

//-----------------------------------------------------------------------//
bool CheckPass(){
  bool check = true;
  if(LOG>1){Serial.println("Check Pass:");}
  for(int i=0;i<4;i++){
    if(LOG>1){Serial.print(MyPass[i],HEX);Serial.print("-");Serial.print(LoraArrayRX[i+7],HEX);}
    if(MyPass[i]!= LoraArrayRX[i+7]){
      if(LOG>1){Serial.println("ERROR");}     
      check = false; 
      break;
    }else{
      if(LOG>1){Serial.println("PASS");}      
    }
  }
  return(check);
}
  
//-----------------------------------------------------------------------//
void LoRa_TX(byte *message, int sizeMsg){
    delay(SendDelay);
    //ret = 0;
    str_ret = "RET: " + String(ret);
    TimerRetry = true;
    lastUpdate_ack = millis();        // azzero timer ricezione ack 
      
    LoRa.beginPacket();
    LoRa.setTxPower(20,RF_PACONFIG_PASELECT_PABOOST);
    for (int i = 0; i < sizeMsg ; i++){ 
      LoRa.write(message[i]);
      //Serial.print(i);Serial.print("-");Serial.println(message[i]);
    }
    LoRa.endPacket();
      
    digitalWrite(LED, HIGH);  // turn the LED on 
    delay(SetLedTmr);
    digitalWrite(LED, LOW);   // turn the LED off 
    
    lastUpdate_ack = millis();  
}

//-----------------------------------------------------------------------//
//  CALCOLO CRC8 POLINOMIO PARTENZA 0X59 (RECUPERATO DA VIVALDI)

// Found on http://www.leonardomiliani.com/en/2013/un-semplice-crc8-per-arduino/
//CRC-8 - based on the CRC8 formulas by Dallas/Maxim
//code released under the terms of the GNU GPL 3.0 license
byte CRC8( const byte *data, byte len ) {

  byte crc = 0x59;
  while (len--) {

    byte extract = *data++;
    for (byte tempI = 8; tempI; tempI--) {
      byte sum = (crc ^ extract) & 0x01;
      crc >>= 1;
      if (sum) {
        crc ^= 0x8C;
      }
      extract >>= 1;
    }
  }
  return crc;
}


//------------------------ STAMPE LOG -----------------------------------//
//-----------------------------------------------------------------------//
void PrintTxPacket(){
  Serial.println("pacchetto da trasmettere: ");
  Serial.print("Funzione:   ");Serial.println(LoraArrayTX[0],HEX);
  Serial.print("SenderID:   ");Serial.println(LoraArrayTX[1],HEX);
  Serial.print("ReceiverID: ");Serial.println(LoraArrayTX[2],HEX);
  Serial.print("key0:       ");Serial.println(LoraArrayTX[3],HEX);
  Serial.print("key1:       ");Serial.println(LoraArrayTX[4],HEX);
  Serial.print("key2:       ");Serial.println(LoraArrayTX[5],HEX);
  Serial.print("key3:       ");Serial.println(LoraArrayTX[6],HEX);
  Serial.print("pass0:      ");Serial.println(LoraArrayTX[7],HEX);
  Serial.print("pass1:      ");Serial.println(LoraArrayTX[8],HEX);
  Serial.print("pass2:      ");Serial.println(LoraArrayTX[9],HEX);
  Serial.print("pass3:      ");Serial.println(LoraArrayTX[10],HEX);
  Serial.print("StatusIn1:  ");Serial.println(LoraArrayTX[11],HEX);
  Serial.print("StatusIn2:  ");Serial.println(LoraArrayTX[12],HEX);
  Serial.print("LoraRssi:   ");Serial.println(LoraArrayTX[13],HEX);
  Serial.print("nErr:       ");Serial.println(LoraArrayTX[14],HEX);
  Serial.print("nFail:      ");Serial.println(LoraArrayTX[15],HEX);  
  Serial.print("crc8:       ");Serial.println(LoraArrayTX[16],HEX);
  Serial.println("myPass: ");
  Serial.print("myPass[0]:   ");Serial.println(MyPass[0],HEX); 
  Serial.print("myPass[1]:   ");Serial.println(MyPass[1],HEX); 
  Serial.print("myPass[2]:   ");Serial.println(MyPass[2],HEX); 
  Serial.print("myPass[3]:   ");Serial.println(MyPass[3],HEX); 
}

//-----------------------------------------------------------------------//
void PrintRxPacket(){
  Serial.println("pacchetto ricevuto: ");
  Serial.print("Funzione:   ");Serial.println(LoraArrayRX[0],HEX);
  Serial.print("SenderID:   ");Serial.println(LoraArrayRX[1],HEX);
  Serial.print("ReceiverID: ");Serial.println(LoraArrayRX[2],HEX);
  Serial.print("key0:       ");Serial.println(LoraArrayRX[3],HEX);
  Serial.print("key1:       ");Serial.println(LoraArrayRX[4],HEX);
  Serial.print("key2:       ");Serial.println(LoraArrayRX[5],HEX);
  Serial.print("key3:       ");Serial.println(LoraArrayRX[6],HEX);
  Serial.print("pass0:      ");Serial.println(LoraArrayRX[7],HEX);
  Serial.print("pass1:      ");Serial.println(LoraArrayRX[8],HEX);
  Serial.print("pass2:      ");Serial.println(LoraArrayRX[9],HEX);
  Serial.print("pass3:      ");Serial.println(LoraArrayRX[10],HEX);
  Serial.print("StatusIn1:  ");Serial.println(LoraArrayRX[11],HEX);
  Serial.print("StatusIn2:  ");Serial.println(LoraArrayRX[12],HEX);
  Serial.print("LoraRssi:   ");Serial.println(LoraArrayRX[13],HEX);
  Serial.print("nErr:       ");Serial.println(LoraArrayRX[14],HEX);
  Serial.print("nFail:      ");Serial.println(LoraArrayRX[15],HEX);  
  Serial.print("crc8:       ");Serial.println(LoraArrayRX[16],HEX);
}
