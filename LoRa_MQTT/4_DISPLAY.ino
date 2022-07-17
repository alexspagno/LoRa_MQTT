
//------------------------ STAGE DISPLAY --------------------------------//
//-----------------------------------------------------------------------//
void displaySenderTxStage1(){
  head_1 = "k ";
  for (int i=3; i<7;i++){
    head_1 += String(LoraArrayTX[i],HEX);
  }  
  head_2 = "";
  text_msg = "Request to send";
  footer_1 = "Tx Req";
  footer_2 = "";
  Display();
}
//-----------------------------------------------------------------------//
void displayReceiverRxStage1(){
  head_1 = "k ";
  for (int i=3; i<7;i++){
    head_1 += String(LoraArrayRX[i],HEX);
  }  
  head_2 = "";
  text_msg = "";
  footer_1 = "Rx Req";
  footer_2 = "";
  Display();
}
//-----------------------------------------------------------------------//
void displayReceiverTxStage1(){
  head_2 = "p ";
  for (int i=7; i<11;i++){
    head_2 += String(LoraArrayTX[i],HEX);
  }
  text_msg = "Confirm request";  
  footer_1 = "Tx Req ack";  
  Display();
}
//-----------------------------------------------------------------------//
void displaySenderRxStage2(){
  head_1 = "k ";
  for (int i=3; i<7;i++){
    head_1 += String(LoraArrayRX[i],HEX);
  }  
  head_2 = "";
  text_msg = "";  
  footer_1 = "Rx Req ack";
  footer_2 = "";
  Display();
}
//-----------------------------------------------------------------------//
void displaySenderTxStage2(){
  head_2 = "p ";
  for (int i=7; i<11;i++){
    head_2 += String(LoraArrayTX[i],HEX);
  }
  footer_1 = "Tx Status";  
  Display();
}  
//-----------------------------------------------------------------------//
void displayReceiverRxStage2(){
  head_1 = "k ";
  for (int i=3; i<7;i++){
    head_1 += String(LoraArrayRX[i],HEX);
  }  
  head_2 = "";
  footer_1 = "Rx Status";
  footer_2 = "";
  Display();
}
//-----------------------------------------------------------------------//
void displayReceiverTxStage2(){
  head_2 = "p ";
  for (int i=7; i<11;i++){
    head_2 += String(LoraArrayTX[i],HEX);
  }    
  footer_1 = "Tx Status ack";  
  Display();
}
//-----------------------------------------------------------------------//
void displaySenderTxStage3(){
  head_1 = "p ";
  for (int i=7; i<11;i++){
    head_1 += String(LoraArrayTX[i],HEX);
  }
  head_2 = "";
  footer_1 = "Rx Status ack";  
  footer_2 = "";
  Display();
}
//-----------------------------------------------------------------------//
void displayMqttRX(){
  head_1 = "MQTT RX";
  head_2 = "";
  footer_1 = "";  
  footer_2 = "";
  Display();
}





//------------------------ GRAFICHE PAGINE ------------------------------//
//-----------------------------------------------------------------------// 
void displayTableStatus(){
  Heltec.display->drawRect(0 , 0  , 128 , 64);
  Heltec.display->drawLine(0 , 48 , 128 , 48);
  Heltec.display->drawLine(90 , 48 , 90 , 64);
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10); 
  if(master){
    Heltec.display->drawString(2, 50, "WIFI");
    Heltec.display->drawProgressBar(29, 52, 58, 8, D_WifiRssiBar);
    Heltec.display->drawString(93, 50, mqttStatus);
  }else{
    Heltec.display->drawString(2, 50, "LoRa");
    Heltec.display->drawProgressBar(29, 52, 58, 8, D_LoraRssiBar);
    Heltec.display->drawString(93, 50, D_LoraRssi);    
  }
  Heltec.display->display(); 
}
//-----------------------------------------------------------------------// 
void displayCalendar(){ 
  String orario = "";
  String now_time = "";
  int hr; // = hour();
  int min;// = minute();
  String str_hr;
  String str_min;  
  
  if(master){

    hr = timeClient.getHours(); 
    min = timeClient.getMinutes();
    str_hr = "";
    str_min = "";
    if (hr<10) str_hr = "0" + String(hr); else str_hr = String(hr);
    if (min<10) str_min = "0" + String(min); else str_min = String(min);
    now_time = str_hr + ":" + str_min;      

    
    //orario = timeClient.getFormattedTime();
    //now_time = orario.substring(0,5);

    
  }else{
    hr = hour();
    min = minute();
    str_hr = "";
    str_min = "";
    if (hr<10) str_hr = "0" + String(hr); else str_hr = String(hr);
    if (min<10) str_min = "0" + String(min); else str_min = String(min);
    now_time = str_hr + ":" + str_min;      
  }
  Heltec.display->clear();
  displayTableStatus();    
  Heltec.display->setTextAlignment(TEXT_ALIGN_CENTER);
  Heltec.display->setFont(ArialMT_Plain_24);  
  Heltec.display->drawString(64, 10, now_time);
  Heltec.display->display();  
}
//-----------------------------------------------------------------------// 
void DisplayConnection(){ 
  Heltec.display->clear();
  displayTableStatus();    
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);  
  Heltec.display->drawString(5, 2, head_1);
  Heltec.display->drawString(5, 20, head_2);
  Heltec.display->drawString(5, 30, head_3);
  Heltec.display->display(); 
  //lastUpdate_display = millis();  
}
//-----------------------------------------------------------------------// 
void Display(){ 
  Heltec.display->clear();
  displayTableStatus();    
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);  
  Heltec.display->drawString(5, 2, head_1);
  Heltec.display->drawString(70, 2, head_2);
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(5, 16, text_msg);
  Heltec.display->setFont(ArialMT_Plain_10); 
  Heltec.display->drawString(5, 34, footer_1); 
  Heltec.display->drawString(70, 34, footer_2); 
  Heltec.display->display(); 
  lastUpdate_display = millis();  
}
//-----------------------------------------------------------------------// 
void displayInformation(){ 
  Heltec.display->clear();
  displayTableStatus();    
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);  
  Heltec.display->drawString(5, 2, head_1);
  Heltec.display->drawString(70, 2, head_2);
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(5, 16, text_msg); 
  Heltec.display->display(); 
  lastUpdate_display = millis();  
}
//-----------------------------------------------------------------------// 
void displayFooter(){ 
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(5, 34, footer_1); 
  Heltec.display->drawString(70, 34, footer_2);  
  Heltec.display->display();  
}
