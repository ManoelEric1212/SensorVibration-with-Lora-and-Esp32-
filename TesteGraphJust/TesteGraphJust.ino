#include <SPI.h> //responsável pela comunicação serial
#include <LoRa.h> //responsável pela comunicação com o WIFI Lora
#include <Wire.h>  //responsável pela comunicação i2c
#include <WiFi.h>  //Criação de ponto de acesso                                                            //Inclusão das bibliotecas
#include <WebServer.h> //Gear o servidor web
#include "SSD1306.h" //responsável pela comunicação com o display
#include "index.h" //Arquivo da página web

// Definição dos pinos 
#define SCK     5    // GPIO5  -- SX127x's SCK
#define MISO    19   // GPIO19 -- SX127x's MISO
#define MOSI    27   // GPIO27 -- SX127x's MOSI
#define SS      18   // GPIO18 -- SX127x's CS
#define RST     14   // GPIO14 -- SX127x's RESET
#define DI00    26   // GPIO26 -- SX127x's IRQ(Interrupt Request)
#define BAND    915E6  //Frequencia do radio - podemos utilizar ainda : 433E6, 868E6, 915E6
#define PABOOST true

//parametros: address,SDA,SCL 
SSD1306 display(0x3c, 4, 15); //construtor do objeto que controlaremos o display
WebServer sv(80);
//Parâmetros da rede wifi que o esp32 irá conectar
const char* ssid     = "Manoel_Eric";
const char* password = "mrrobot12";
String rssi = "RSSI --";
String packSize = "--";
String packet;
String acx,acy,acz;
String indica_normal,indica_dist1,indica_dist2,indica_dist3,indica_dist4;
String cont_dist1;
String id_equipamento;

void setup() {
  Serial.begin(9600);
  setupDisplay();
  loraComunication();
  webServerConnect();
}

void loop() {
  recebePacote();
  delay(2);
  sv.handleClient();
  delay(1);
}
// Função responsável por configurar os dados que serão exibidos em tela.
void loraData(){
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, String(rssi));
  display.drawString(0, 15,"ACX: " + String(acx));
  display.drawString(0, 30,"ACY: " + String(acy));
  display.drawString(0, 45,"ACZ: " + String(acz));  
  display.display();
}

// Recebe os dados do Slave e Transforma no JSON
void recebePacote(){
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    while (LoRa.available()) {
    acx = LoRa.readStringUntil('|');
    acy = LoRa.readStringUntil('|');
    acz = LoRa.readStringUntil('|');
    indica_normal = LoRa.readStringUntil('|');
    indica_dist1 = LoRa.readStringUntil('|');
    id_equipamento = LoRa.readStringUntil('|');
    rssi = "RSSI=  " + String(LoRa.packetRssi(), DEC)+ "dB";  
    
    String data_sensors = "{ \"Acx\": " + acx + "," + "\"Acy\": " + acy + "," + "\"Acz\": " + acz + " }";
    String indica_disturbio = "{ \"Indica_normal\": " + indica_normal + "," + "\"Indica_dist1\": " + indica_dist1 + "}";
    String id_equipamento_maq = "{ \"Id_quipamento\": "+ id_equipamento + "}";
    }
  // Mostra no display
  loraData();
  Serial.println("{ \"Id_equipamento\": "+ id_equipamento + "}"); 
  }
}

void setupDisplay(){
  //configura os pinos como saida
  
  pinMode(16,OUTPUT); //RST do oled
  digitalWrite(16, LOW);    // reseta o OLED
  delay(50); 
  digitalWrite(16, HIGH); // enquanto o OLED estiver ligado, GPIO16 deve estar HIGH
  display.init();
  display.flipScreenVertically();  
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "SENSOR VIBRAÇÃO");
  delay(1500);
  display.clear();
  
  }
// Função que inicializa a comunicação LoRa
void loraComunication(){
  SPI.begin(SCK,MISO,MOSI,SS); //inicia a comunicação serial com o Lora
  LoRa.setPins(SS,RST,DI00); //configura os pinos que serão utlizados pela biblioteca (deve ser chamado antes do LoRa.begin)
  //inicializa o Lora com a frequencia específica.
  if (!LoRa.begin(BAND)) {
    display.drawString(0, 0, "Comunicação Lora falhou !");
    display.display();
    while (1);
  }

// Indica no display que inicilizou corretamente.
  display.drawString(0, 0, "LoRa iniciou com sucesso !!");
  display.drawString(0, 10, "Esperando por dados ...");
  display.display();
  delay(1000);
  LoRa.receive();
  
  }
  
void webServerConnect(){
  Serial.print("Se conectando a: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password); //Se conecta ao Wi-Fi
  while (WiFi.status() != WL_CONNECTED) { //Verifica se a conexão foi bem-sucedida
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP()); //Imprime o endereço de IP
  sv.on("/", handleRoot);
  sv.on("/acx", handle_acx);
  sv.on("/leituras", handle_leituras);
  sv.on("/botoes", handle_disturbios);
  sv.on("/id", handle_id);
  sv.onNotFound(nao_encontrado);
  sv.begin(); //Inicia o servidor
  Serial.println("Servidor Online");
  
  }

//Construção página web
void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 sv.send(200, "text/html", s); //Send web page
}
// Parte responsável pela atualização via Ajax
void handle_acx()
{
  String data_sensors = "{ \"Acx\": " + acx + "," + "\"Acy\": " + acy + "," + "\"Acz\": " + acz + " }";
  sv.send(200, "text/plane", data_sensors);
}

void handle_leituras()
{
  String data_sensors = "{ \"Acx\": " + acx + "," + "\"Acy\": " + acy + "," + "\"Acz\": " + acz + " }";
  sv.send(200, "text/plane", data_sensors);
}

void handle_disturbios()
{
  String indica_disturbio = "{ \"Indica_normal\": " + indica_normal + "," + "\"Indica_dist1\": " + indica_dist1 + "}";
  sv.send(200, "text/plane", indica_disturbio);
}
void handle_id()
{
  String id_equipamento_maq = "{ \"Id_equipamento\":  \" " + id_equipamento + " \" }";
  sv.send(200, "text/plane", id_equipamento_maq);
}

// Retorna um erro caso a página não seja encontrada
void nao_encontrado() {                                                           //Sub-rotina para caso seja retornado um erro
  sv.send(404, "text/plain", "Não encontrado");                                   //Retorna a mensagem de erro em caso de um retorno 404
}
