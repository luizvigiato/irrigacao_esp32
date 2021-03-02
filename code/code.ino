#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>


WiFiManager wifiManager;

WiFiServer server(80);
WiFiClient client;

String header;

#define temp 19
#define humi 5
#define led_modulo 2


int temperatura;
int humidade = 600;
int contagem = 0;
int media = 0;
int registro;

void setup() {
  wifiManager.autoConnect("Irrigacao LH v.2");
  Serial.begin(115200);
  EEPROM.begin(256);
  pinMode(temp, INPUT);
  pinMode(humi, INPUT);
  pinMode(led_modulo, OUTPUT);
  

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
  if(contagem > 60){
    humidade = media / 60;
    contagem = 0;
  } else {
    media += analogRead(humi);

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
                "<title>Sistema de Irrigacao</title><meta charset=\"utf-8\"></head><body>"
                "<center><h1>Sistema de Irrigacao Vigiato v.2</h1>"
                "<h2>Temperatura atual: ");
  String color = "red";

  htmlPage += (String) contagem;
  htmlPage +=   " c </h2><h3>Status de Irrigação: <em style=\"color:";
  htmlPage += color;
  htmlPage +=   "\">OFFLINE</em></h3>";
  return htmlPage;  
}
