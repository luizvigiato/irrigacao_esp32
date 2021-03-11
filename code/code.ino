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
int min_humidade = 0;
int contagem = 0;
int media = 0;
int qnt_tempo = 0;
bool info_status = false;

void setup() {
  wifiManager.autoConnect("Irrigacao LH v.2");
  Serial.begin(115200);
  EEPROM.begin(256);
  min_humidade = EEPROM.read(0)*4;
  humidade = EEPROM.read(1)*4;
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
  Serial.println(media);
  if(contagem > 60){
    humidade = media / 60;
    EEPROM.write(1,humidade/4);
    EEPROM.commit();
    media = 0;
    contagem = 0;
  } else {
    media += analogRead(sensor);

  }

  if(humidade > min_humidade){
    info_status = true;
  } else {
    info_status = false;
  }
    

  if(client){
    Serial.println("Cliente acessando");

    String request = client.readStringUntil('\r');
    int valor_min = request.indexOf("/?min=");
    if(valor_min > 0){
      
      String teste = String(request[valor_min+6]);
      teste += String(request[valor_min+7]);
      teste += String(request[valor_min+8]);
      min_humidade = teste.toInt();
      EEPROM.write(0, min_humidade/4);
      EEPROM.commit();
      
    }
    
    
    
    while(client.available()){
      char c = client.read();
      header += c;
    }

    client.flush();
    Serial.println("Impressao pagina");
    client.println(textoHtmlPagina());
  }
  
  
  contagem++;
  if(info_status) qnt_tempo++;
  else qnt_tempo = 0;
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
//                "<link rel=\"stylesheet\" "
//                "href=\"https://cdn.jsdelivr.net/npm/bootstrap@4.6.0/dist/css/bootstrap.min.css\" "
//                "integrity=\"sha384-B0vP5xmATw1+K9KRQjQERJvTumQW0nPEzvF6L/Z6nronJ3oUOFUFpCjEUQouq2+l\" "
//                "crossorigin=\"anonymous\">"
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
    htmlPage += qnt_tempo;
    htmlPage += F("</td></tr></table>");
    
  } else {
    htmlPage += F("</tr></table>");
  }

  htmlPage += F(
              "</div><div style=\"margin-top: 35px;\"><em style=\"font-size: 2em;\">"
              "<b>Informações do Equipamento</b></em>"
              "<table style=\"margin-top:8px; font-size: 1.5em;\"><tr></tr>"
              "<tr><td>Wi-Fi conectado: </td><td>"
    );

  htmlPage += WiFi.SSID();

  htmlPage += F(
     "</td></tr><tr><td>Codigo equipamento:  </td><td style=\"color:red\"><b>"
  );

  htmlPage += WiFi.softAPmacAddress();

  htmlPage += F(
    "</b></td></tr></table>"
    "</div><div style=\"margin-top: 35px;\">"
    "<em style=\"font-size: 2em;\"> <b>Informações do Sistema</b></em>"
      "<table style=\"margin-top:8px; font-size: 1.5em;\">"
        "<tr><td>Nivel de umidade atual:</td><td>"
    );

  htmlPage += (String)humidade;

  htmlPage += F(
    "</td></tr><tr><td>Valor minimo umidade:</td><td>"
    );

  htmlPage += (String)min_humidade;

  htmlPage += F(
    "</td></tr>"
    "<tr><form method=\"get\"><td>Alterar Minimo: "
    "<input type=\"text\" name=\"min\" maxlength=\"3\" value=\"");

  htmlPage += (String)min_humidade;
    
  htmlPage += F(
    "\" style=\"width:3em; font-size:0.9em;\"></td>"
    "<td><button type=\"submit\" style=\"background-color:#2E8B57; font-size:1em; border: none; height:100%; color: white; padding: 3px 10px;\">"
    "Alterar</button></form></td></tr></table></div>"
    );
  
  
  return htmlPage;  
}
