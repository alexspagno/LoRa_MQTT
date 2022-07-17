/* 
 *  
 * controllare impostazione LOG su Serial.print
 * comandi toggle da mqtt!!
 * invio LoRa cambiamento stati e check Slave, non viene trasmetto inditreo a seguito attivazione uscita o check
 *  
 *  #### WIFI ####
 * Se non va wifi dopo un TOT di tentativi rifaccio procedura o ESP.reset()
 * multi SSID
 * 
 * #### LORA ####
 * messaggio se non vedo master
 * messaggio se non vedo slave
 * integrare invio LoRa e MQTT nERR nFAIL // attenzione byte solo 256 come val max
 * invio sopravvivenza
 * invio orario ( ora legale )
 * svincolare trasmettitore dopo lo scadere del controllo ACK TIMER... tipo ACK TIMER + 1000
 * 
 * #### STATI ####
 * //OK da testare // stato output!!!
 * ////////////////////////// NON IMPLEMENTATO CONTROLLO STATO USCITA!
 * 
 * #### AVVIO ####
 * Slave:
 * -invio stati via LoRa
 * 
 * Master:
 * -invio orario
 * //OK da testare //-invio stati via LoRa
 * -richiedo stati via LoRa
 * //OK da testare //-invio tutti gli stati a MQTT 
 * 
 * #### MQTT ####
 * //OK da testare //richiesta stato con check ( controllare)
 * //OK da testare //da rifare tutta la parte comandi
 * Uscite monostabili o statiche
 * per uscite monostabili creare TIMER * 
 * gestire l'eventuale mancato invio.... potrebbe bloccare trasmissione LoRa
 *  
 * Contatore errori e trasmissioni fallite
 * - 1 messaggio di controllo ogni 5 minuti quindi 12 messaggi ora. 
 *  Ogni ora invio numero di errori effettuati da visualizzare su grafico.
 * - memorizzo 100 trasmissioni e quantità errori a scorrimento, ogni messaggio aggiorno la media eliminado il piu vecchio.
 *  Creo un arrai di valori dimensione 100
 * 
 * #### LOGO ####
 * aggiustare logo.. imperfetto!
 */

 

 
