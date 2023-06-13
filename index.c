#include <HX711.h>
#include <ESP8266WiFi.h>

// DEFININDO OS PINOS
#define pinDT  D2
#define pinSCK D3
#define pinBotao D4

// INSTANCIANDO OS OBJETOS
HX711 scale;

// DECLARANDO VARIÁVEIS
float medida = 0;

// CONFIGURAÇÕES DO WI-FI
const char* ssid = "";
const char* password = "";
const char* serverIP = "";


WiFiClient client;

void setup() {
  Serial.begin(115200);

  // LOOP DA CONEXÃO DO WIFI
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando à rede Wi-Fi...");
  }
  Serial.println("Conectado à rede Wi-Fi!");

  scale.begin(pinDT, pinSCK); 
  scale.set_scale(); // DEFININDO A TARA

  delay(2000);
  scale.tare(); // ZERANDO A BALANÇA JÁ COM A CALIBRAGEM

  Serial.println("Balança Zerada");
}

void loop() {
  medida = scale.get_units(5); 
  Serial.println(medida, 3); // ENVIANDO PARA MONITOR SERIAL A MEDIDA COM 3 CASAS DECIMAIS

  if (client.connect(serverIP, 80)) {
    // ENVIAR MEDIDA PARA O SERVIDOR
    String payload = "medida=" + String(medida, 3);
    client.println("POST /enviar_medida HTTP/1.1");
    client.println("Host: " + String(serverIP));
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(payload.length());
    client.println();
    client.println(payload);
    client.println();
    client.stop();
  } else {
    Serial.println("Falha ao se conectar ao servidor!");
  }

  scale.power_down(); // DESLIGANDO O SENSOR
  delay(1000); // AGUARDA 1 SEGUNDO
  scale.power_up(); // LIGANDO O SENSOR
}
