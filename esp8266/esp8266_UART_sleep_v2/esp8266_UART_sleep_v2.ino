/*
 esp8266 (Deep Sleep)+ Atmega328 UART connect
 LM60BIZ Sensor.
*/

#include <ESP8266WiFi.h>
//#include <DHT.h>
extern "C" {
#include <user_interface.h>
}

const char* ssid = "";
const char* password = "";

const char* host = "api.thingspeak.com";
String mAPI_KEY="your-Key";

//
void setup() {
  // put your setup code here, to run once:
  Serial.begin( 9600 );
  Serial.println("#Start-setup");
  Serial.print("millis.Start: ");
  Serial.println(millis() );

  //dht.begin();
  delay(10);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("millis.Connected: ");
  Serial.println(millis() );
}

//
void proc_http(String sTemp ){
//Serial.print("connecting to ");
//Serial.println(host);  
      WiFiClient client;
      const int httpPort = 80;
      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
      }
      //String url = "/update?key="+ mAPI_KEY + "&field1="+ sTemp +"&field2=" + sHum; 
      String url = "/update?key="+ mAPI_KEY + "&field1="+ sTemp;        
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
        "Host: " + host + "\r\n" + 
        "Connection: close\r\n\r\n");
      delay(10);      
      int iSt=0;
      while(client.available()){
          String line = client.readStringUntil('\r');
Serial.print(line);
      }    
}
//
void loop() {
  int iSleepSec= 900;
  delay( 100 );
Serial.print("millis.Loop: ");
Serial.println(millis() );
  if (millis() > 30000 ){
       ESP.deepSleep( iSleepSec * 1000 * 1000);
  }
  
  String sRecv="";
  while( Serial.available()){
    char c = Serial.read();
    sRecv.concat(c);
    //if(  sRecv.length() >= 9 ){
    if(  sRecv.length() >= 6 ){
      String sHead= sRecv.substring(0,4);
      //if(sHead =="dat="){
      if(sHead =="tmp="){
        String sTmp = sRecv.substring( 4, 6 );
//        String sHum = sRecv.substring( 6, 9 );        
Serial.print("sTmp=");
Serial.println( sTmp );       
//Serial.print("sHum=");
//Serial.println( sHum );
        proc_http(sTmp );
//        ESP.deepSleep( 30 * 1000 * 1000);
        ESP.deepSleep( iSleepSec * 1000 * 1000);
      }
    }
  }
  
}


