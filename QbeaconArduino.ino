#include <SoftwareSerial.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include "BluetoothLE.h"

#define BEACONTOPIC "BEACON01"
#define SALATOPIC "UFC/QUIXADA/BLOCO01/SALA01"

int lampada = 8;

// Atualize estes com valores adequados para sua rede.
byte mac[]    = {  
  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 105);
IPAddress server(192, 168, 1, 100);

BluetoothLE ble(5,6);
EthernetClient ethClient;
PubSubClient client(ethClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String menssagem = "";
  String topico = "";
  
  for (int i=0;i<(int)strlen(topic);i++) {
    topico += topic[i];
  }
  
  for (int i=0;i<length;i++) {
    menssagem += (char)payload[i];
  }
  
  Serial.println(menssagem);
  
  if(topico == BEACONTOPIC){
    ble.setUUID(0, menssagem.substring(0, 8));
  }
  
  if(topico == SALATOPIC){
    if(menssagem == "1"){
      digitalWrite(lampada, HIGH);
      delay(3000);
      Serial.println("LIGOU O LED");
    }
    if(menssagem == "0"){
      digitalWrite(lampada, LOW);
      delay(3000);
      Serial.println("DESLIGOU O LED");
    }
  }
 
  Serial.println();
}

void reconnect(){
  //Faça um loop até nos reconectarmos
  while (!client.connected()) {
    Serial.print("Tentando uma conexao MQTT...");
    // Tentativa de conectar
    if(client.connect("arduinoClient")){
      Serial.println("Conectado!");
      // ... e se inscrever novamente
      client.subscribe(BEACONTOPIC);
      client.subscribe(SALATOPIC);
    }
    else {
      Serial.print("Erro ao se conectar, rc=");
      Serial.print(client.state());
      Serial.println("Tentando conectar novamente em 5s...");
      // Espere 5 segundos e tente novamente.
      delay(5000);
    }
  } 
}

void setup()
{
  Serial.begin(9600);
  pinMode(led, OUTPUT);

  Serial.println("Iniciando Bluetooth...");
  ble.begin(9600);
  Serial.println("Bluetooth iniciado!");
  delay(1000);

  Serial.println("Iniciando Ethernet...");
  client.setServer(server, 1883);
  client.setCallback(callback);
  Ethernet.begin(mac, ip);
  Serial.println("Ethernet iniciado!");
  // Permitir que o hardware se classifique
  delay(1500);
}

void loop()
{
  
  if (!client.connected()) {
    reconnect();
  }

  ble.loop();

  if(ble.hasMessage()) {
    Serial.print("BLE MENSAGEM: ");
    Serial.print(ble.getMessage());
    Serial.println();
  }

  client.loop();

}

