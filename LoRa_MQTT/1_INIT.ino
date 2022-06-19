// ROUTINE INIZIALIZZAZIONE

//-----------------------------------------------------------------------//
void  heltec_init(){
  // serial 115200!
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
}

//-----------------------------------------------------------------------//
void display_init() {
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_10);
  logo();
  delay(1500); 
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(5, 5, "Versione:");
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(5, 26, ino);
  Heltec.display->display();
  delay(2000);  
  //displayInformation("","","INIT DISPLAY");
  head_1 = "";  
  head_2 = "";
  text_msg = "INIT DISPLAY";
  footer_1 = "";
  footer_2 = "";
  Display();
  delay(1000);    
  }
void logo()
{
  Heltec.display->clear();
  Heltec.display->drawXbm(0,5,logo_width,logo_height,logo_bits);
  Heltec.display->display();
}

//-----------------------------------------------------------------------//
void lora_init(){
  LoRa.receive();
  //displayInformation("","","INIT LoRa");
  head_1 = "";  
  head_2 = "";
  text_msg = "INIT LoRa";
  footer_1 = "";
  footer_2 = "";
  Display(); 
  delay(1000);
}


//-----------------------------------------------------------------------//
void wifi_init() {
  Serial.println("LOG: WIFI - OFFLINE");
  delay(10);
  //WriteDisplay("WifiConnection");
  head_1 = "Try Connecting to: ";  
  head_2 = ssid;
  head_3 = "";
  DisplayConnection();
    
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  WiFi.begin(ssid, password);
  Serial.print("LOG: WIFI - Connecting ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }  
  Serial.println("");
  Serial.println("LOG: WIFI - Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  local_ip = WiFi.localIP().toString();

  head_1 = "WiFi connected";  
  head_2 = "IP address:";
  head_3 = local_ip;
  DisplayConnection(); 
  
  delay(1000); 
}

//-----------------------------------------------------------------------//
void ntp_init(){
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(3600);
}  



//-----------------------------------------------------------------------//
void mqtt_init(){
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
