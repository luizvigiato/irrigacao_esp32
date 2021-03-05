#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>


WiFiManager wifiManager;

WiFiServer server(80);
WiFiClient client;

String header;

#define LM35 19
#define sensor 5
#define led_modulo 2


int temperatura;
int humidade = 600;
int contagem = 0;
int media = 0;
int registro;
bool info_status = false;

void setup() {
  wifiManager.autoConnect("Irrigacao LH v.2");
  Serial.begin(115200);
  EEPROM.begin(256);
  pinMode(LM35, INPUT);
  pinMode(sensor, INPUT);
  
  delay(5000);
  // Boot service irrigation
  Serial.println("Iniciando");
  //print on serial connection status
  Serial.println(wifiManager.autoConnect());
//  Serial.println(wifiManager.getWiFiIsSaved());
//  Serial.println(wifiManager.getWiFiSSID());

  server.begin();
  digitalWrite(led_modulo,LOW);

  

}

void loop() {
//  identify connection status
//  Serial.println(wifiManager.autoConnect());
//  digitalWrite(led_modulo,HIGH);

  client = server.available();
  if(contagem > 60){
    humidade = media / 60;
    media = 0;
    contagem = 0;
  } else {
    media += analogRead(sensor);

  }
    

  if(client){
    Serial.println("Cliente acessando");

    String request = client.readStringUntil('\r');

    
    while(client.available()){
      char c = client.read();
      header += c;
    }

    client.flush();
    Serial.println("Impressao pagina");
    client.println(textoHtmlPagina());
  }
  
  
  contagem++;
  delay(1000);

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
                "<title>Sistema de Irrigacao</title><meta charset=\"utf-8\"></head><body style=\"background-color:#F0FFFF\">"
                "<center><h1>Sistema de Irrigacao Vigiato v.2.2</h1>"
                "</br></br>"

                "<div>"
                "<table style=\"font-size: 1.5em;\">"
                "<tr >"
                "<td>Temperatura atual:</td>"
                "<td> ");
//  temperatura = analogRead(LM35)/1024.0;
  temperatura = (float(analogRead(LM35))*5/(1023))/0.01;

  htmlPage += (String) temperatura;

  htmlPage += F(
    " c</td></tr><tr><td>Status de Irrigação: </td><td><em style=\"color:"
    );

  if(info_status){
    htmlPage += (String)"green";
  } else {
    htmlPage += (String)"red";
  }
  htmlPage += F(";\"><b>");

  if(info_status){
    htmlPage += "ATIVO";
  } else {
    htmlPage += "DESLIGADO";
  }
    
  htmlPage += F("</b></em></td>");
  if(info_status){
    htmlPage += F("<tr><td>Tempo de Irrigação: </td><td>");
    htmlPage += contagem;
    htmlPage += F("</td></tr></table>");
    
  } else {
    htmlPage += F("</tr></table>");
  }

  
  return htmlPage;  
}
