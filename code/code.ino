#include <WiFiManager.h>
#include <ESP8266WiFi.h>


WiFiManager wifiManager;

WiFiServer server(80);
WiFiClient client;

String header;

int temperatura;
int humidade;
int contagem;

void setup() {
  wifiManager.autoConnect("Irrigacao LH v.2");
  Serial.begin(115200);

  delay(5000);
  // Boot service irrigation
  Serial.println("Iniciando");
  //print on serial connection status
  Serial.println(wifiManager.autoConnect());
//  Serial.println(wifiManager.getWiFiIsSaved());
//  Serial.println(wifiManager.getWiFiSSID());

  server.begin();
  

}

void loop() {
//  identify connection status
//  Serial.println(wifiManager.autoConnect());

  client = server.available();
  

  if(client){
    Serial.println("Cliente acessando");

    String request = client.readStringUntil('r');

    
    while(client.available()){
      char c = client.read();
      header += c;
    }

    client.flush();

//    Serial.println(textoHtmlPagina());
    Serial.println("Impressao pagina");
    client.println(textoHtmlPagina());
//    client.stop();
//    Serial.println(request);
//    Serial.println(header);
  }
  

}

String textoHtmlPagina(){
  String htmlPage;
  htmlPage.reserve(1024);

  htmlPage = F("HTTP/1.1 200 OK\r\n"
               "Content-Type: text/html\r\n"
               "Connection: close\r\n"  // the connection will be closed after completion of the response
               "\r\n");            
  htmlPage += F("<!DOCTYPE HTML>"
                "<html lang=\"pt-BR\"><head>"
                "<title>Sistema de Irrigacao</title></head><body>"
                "<h1><center>Sistema de Irrigacao Vigiato v.2<h1>");
  return htmlPage;  
}
