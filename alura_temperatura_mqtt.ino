// Projeto desenvolvido em curso da Alura.
// Mais anotações no Notion: https://sturdy-ginger-704.notion.site/NodeMCU-e-Esp8266-medindo-e-publicando-dados-com-MQTT-cd892191162849f9828b1c689cfac337


// --- WIFI ---
#include <ESP8266WiFi.h>
const char* ssid = "SSID_da_rede";          // troque pelo nome da sua rede
const char* password = "senha_da_rede";     // troque pela senha da sua rede
WiFiClient nodemcuClient;

// --- MQTT ---
#include <PubSubClient.h>
const char* mqtt_Broker = "iot.eclipse.org";           //define o endereço do broker
const char* mqtt_ClientID = "termometro-remerces01";   //define um nome amigável para o cliente MQTT
PubSubClient client(nodemcuClient);                    //cria um objeto que deve estar associado a uma wifi
const char* topicoTemperatura = "rmerces/temperatura"; //cria o tópico para publicar
const char* topicoUmidade = "rmerces/umidade";         //cria o tópico para publicar

// --- DHT ---
#include <DHT.h>
#define DHTPIN D3
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
int umidade;
int temperatura;

// --- DISPLAY ---
#include <Adafruit_SSD1306.h>
#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(OLED_RESET);

// --- SETUP ---
void setup() {
  configurarDisplay();                  //configura o display
  conectarWifi();                       //conecta na wifi
  client.setServer(mqtt_Broker, 1883);  //inicializa o servidor MQTT
}

// --- LOOP ---
void loop() {
  if (!client.connected()) {
    reconectarMQTT(); //caso não esteja conectado com o broker, irá chamar a função para conectar antes de executar o demais passos
  }
  medirTemperaturaUmidade(); 
  mostrarTemperaturaUmidade();
  publicarTemperaturaUmidadeNoTopico();
}

//--- CONECTA NA WIFI ---
void conectarWifi() {
  delay(10);

  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("Conectando ");
  display.display();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.print(".");
    display.display();
  }
}

//--- RECONECTA MQTT CLIENT ---
void reconectarMQTT() {
  while (!client.connected()) {
    client.connect(mqtt_ClientID); //função reconcta o MQTT caso a conexão caia
  }
}

//--- CONFIGURA DISPLAY ---
void configurarDisplay() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.clearDisplay();
}

//--- PUBLICA (MQTT) TEMPERATURA E UMIDADE ---
void publicarTemperaturaUmidadeNoTopico() {
  client.publish(topicoTemperatura, String(temperatura).c_str(), true); //a ultima flag ("true") faz com que o broker exiba a ultima mensagem publicada
  client.publish(topicoUmidade, String(umidade).c_str(), true); //a ultima flag ("true") faz com que o broker exiba a ultima mensagem publicada
}

// --- ONTÉM TEMPERATURA E UMIDADE---
void medirTemperaturaUmidade() {
  umidade = dht.readHumidity();
  temperatura = dht.readTemperature(false);
  delay(5000);
}

// --- ESTRUTURA MENSAGEM PARA DISPLAY ---
void mostrarTemperaturaUmidade() {
  mostrarMensagemNoDisplay("Temperatura", (temperatura), " C");
  mostrarMensagemNoDisplay("Umidade", (umidade), " %");
}

void mostrarMensagemNoDisplay(const char* texto1, int medicao, const char* texto2) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(texto1);
  display.setTextSize(5);
  display.setCursor(20, 20);
  display.print(medicao);
  display.setTextSize(2);
  display.print(texto2);
  display.display();
  delay(2000);
}
