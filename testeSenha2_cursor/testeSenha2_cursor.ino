
//Define os botões de set e enter do sensor slave
#define button_set 12
#define button_enter 13

#include <SPI.h> //responsável pela comunicação serial
#include <U8x8lib.h> //responsável por mostrar algo no display da placa
#include <SPI.h> //responsável pela comunicação serial
#include <LoRa.h> //responsável pela comunicação com o WIFI Lora
#include <Wire.h>  //responsável pela comunicação i2c
#include <ezOutput.h> // responsável pelo controle de clock de um led

//Construtor do display
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16); //Construtor do display 

// Definição dos pinos para uso do LoRa
#define SCK     5    // GPIO5  -- SX127x's SCK
#define MISO    19   // GPIO19 -- SX127x's MISO
#define MOSI    27   // GPIO27 -- SX127x's MOSI
#define SS      18   // GPIO18 -- SX127x's CS
#define RST     14   // GPIO14 -- SX127x's RESET
#define DI00    26   // GPIO26 -- SX127x's IRQ(Interrupt Request)
#define BAND    915E6  //Frequencia do radio - podemos utilizar ainda : 433E6, 868E6, 915E6
#define PABOOST true

const int MPU_addr=0x68;  // Endereço i2c do sensor

int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ; // Variáveis que podem ser lidas no sensor

//vetores utilizados para armazenar dados lidos por um tempo enquanto o sensor é calibrado antes de operar
float cal_acx[150]; 
float cal_acy[150];
float cal_acz[150];
//Variáveis utilizadas para salvar os valores máximos e mínimos lidos

float max_acx,max_acy,max_acz,min_acy,min_acx,min_acz;
//Variáveis auxiliares para controlar a ordem de chamada das funções

int aux_1, aux_2, aux_3;
//Variáveis para indicar se o monitoramento está ok ou não
int indica_normal,indica_disturbio1; 

//Definição  dos pinos para o Led
int led_vermelho = 32; // Led vermelho
int led_amarelo = 33; // Led amarelo

//Vetor de busca para construção do teclado com dois botões e vetor para definição do id
char busca[37] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','0','1','2','3','4','5','6','7','8','9','-'};
char id_equip[9] = {'*','*','*','*','*','*','*','*','*'};
int busca_2[10] = {0,1,2,3,4,5,6,7,8,9};

//Vetores para o cursor do teclado para indicar na tela 
char cursor_id[11] = {'-',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
char cursor_tempo[8] = {'-',' ',' ',' ',' ',' ',' ',' '};
//Contador para o Loop do teclado
int contador[6] = {0,0,0,0,0,0};
//Contador para o Loop de calibração
int cont_calibra;

//Variáveis booleanas para controlar a ordem dos eventos iniciais de settings
bool cond_1 = true;
bool cond_2 = true;
bool cond_3 = true;
bool cond_4 = true;

//Variáveis para os dois botões para o teclado
int set_state;
int enter_state;
//String para armazenar o id digitado
String id_equipamento1;

//Parâmetros para um clock do led de funcionamento 
ezOutput led(led_amarelo); 

void setup() {
  Serial.begin(9600);
  //Inicialização do LoRa
  loraInit();
  //Declarações para os pinos dos leds e botões
  pinMode(button_set, INPUT);
  pinMode(button_enter, INPUT);
  pinMode(led_vermelho, OUTPUT);
  pinMode(led_amarelo, OUTPUT);
  //Inicialização da lib para o display
  u8x8.begin();
  //Delay de controle
  delay(1500);
 
}
void loop() {
  while(cond_4){
    digitalWrite(led_amarelo, HIGH);
    //Função para digitar uma senha de acesso ao settings
    senhaAcesso();
    //Função para escolher o id da máquina
    escolheId();
    delay(1000);
    //Função que faz a calibração do sensor
    calibraSensor();
    delay(1000);
    //Função que recebe o tempo de leitura e faz um contador para ler o padrão 
    tempoLeitura();
    delay(1000);
    cond_4 = false;
    }
  //Função responsável por ler os dados já em estado de monitoramento
  read_sensor();
  led.loop(); 
  pisca_led();
  //normal();
  //disturbio_1();
  //Função responsável por enviar os dados lidos para o MASTER 
  loraSentPacket_data();
  
}


void senhaAcesso(){
    while(cond_1){
      set_state = digitalRead(button_set);
      enter_state = digitalRead(button_enter);
      u8x8.setFont( u8x8_font_torussansbold8_r);
      u8x8.drawString(0,4, "Informe a senha: ");
      
      if(set_state == HIGH && enter_state == HIGH){
          u8x8.clearDisplay();
          u8x8.setFont( u8x8_font_torussansbold8_r);
          u8x8.drawString(0,4, "Desbloqueado !!");
          delay(3000);
          cond_1 = false;
          u8x8.clearDisplay();
        }
      }
   
}
void escolheId(){
  while(cond_2){
    int aux;
    int x = 0;
    int y = 0;
    int contador = 0;
    while(x < sizeof(id_equip)){ // alterei para -1
      set_state =   digitalRead(button_set);
      enter_state = digitalRead(button_enter);
      u8x8.setFont( u8x8_font_torussansbold8_r);
      u8x8.setCursor(1, 0);
      u8x8.print("Informe o ID:");
      u8x8.setCursor(0, 3);
      u8x8.print(String(id_equip[0])+String(id_equip[1])+"-"+String(id_equip[2])+String(id_equip[3])+String(id_equip[4])+String(id_equip[5])+"-"+String(id_equip[6])+String(id_equip[7])+String(id_equip[8]));
      u8x8.setCursor(0,4);
      u8x8.print(String(cursor_id[0])+String(cursor_id[1])+String(cursor_id[2])+String(cursor_id[3])+String(cursor_id[4])+String(cursor_id[5])+String(cursor_id[6])+String(cursor_id[7])+String(cursor_id[8])+String(cursor_id[9])+String(cursor_id[10]));
      
      if(set_state == HIGH && y < 37){ // trocar por <=
        id_equip[x] = busca[y];
        y++;
        delay(200);
        Serial.print("X= ");Serial.println(x);
        Serial.print("Y= ");Serial.println(y);
        if(y > 36){
          y = 0;
          u8x8.clearDisplay(); // Teste de resolução do probelma de amostragem múltipla da imagem
          }
        }
        else if (enter_state == HIGH && x < 9){ // TROCAR POR 9
         x = x + 1;
         y = 0;

         if (x < 2) {
            cursor_id[x-1] = ' ';
            cursor_id[x] = '-';
            }
         if (x == 2){
            cursor_id[x + 1] = '-';
            cursor_id[x-1] = ' '; 
         }
        if (x > 2 && x < 6){
          cursor_id[x] = ' ';
          cursor_id[x+1] = '-';
          }
        if ( x == 6){
          cursor_id[x] = ' ';
          cursor_id[x+2] = '-';
          
          }
        if ( x > 6){
          cursor_id[x+1] = ' ';
          cursor_id[x+2] = '-';
          }

         delay(200);
         Serial.print("X= ");Serial.println(x);
         Serial.print("Y= ");Serial.println(y);
        }
        else if (set_state == HIGH && enter_state == HIGH){ // VERIFICAR ESTAS CONDIÇÕES
          }
      }
      cond_2 = false;
      u8x8.clearDisplay();
      u8x8.setFont( u8x8_font_torussansbold8_r);
      u8x8.setCursor(2, 1);
      u8x8.print("Finalizou !!!");
      u8x8.setCursor(2, 3);
      u8x8.print("Id escolhido: ");
      u8x8.setCursor(1, 5);
      u8x8.print("S"+String(id_equip[1])+"-"+String(id_equip[2])+String(id_equip[3])+String(id_equip[4])+String(id_equip[5])+"-"+String(id_equip[6])+String(id_equip[7])+String(id_equip[8]));
      delay(3000);
      u8x8.clearDisplay();
    }
  }

void tempoLeitura(){
  int aux2;
  while(cond_3){
    int x_1 = 0;
    int y_1 = 0;
    while(x_1 < 6){ // TrOQUEI POR  por 6 
      set_state =   digitalRead(button_set);
      enter_state = digitalRead(button_enter);
      u8x8.setFont( u8x8_font_torussansbold8_r);
      u8x8.drawString(0,0, "Tempo de leitura:");
      u8x8.setCursor(1, 3);
      u8x8.print(String(contador[0]) + String(contador[1]) + ':' + String(contador[2]) + String(contador[3]) + ':' + String(contador[4]) + String(contador[5]));
      u8x8.setCursor(1, 4);
      u8x8.print(String(cursor_tempo[0]) + String(cursor_tempo[1])+ String(cursor_tempo[2])+ String(cursor_tempo[3])+ String(cursor_tempo[4])+ String(cursor_tempo[5])+ String(cursor_tempo[6])+ String(cursor_tempo[7]));
      
      if(set_state == HIGH && y_1 < 10){
        contador[x_1] = busca_2[y_1];
        y_1++;
        delay(200);
        if(y_1 > 10){
          y_1 = 0;
          u8x8.clearDisplay(); // Teste de resolução do probelma de amostragem múltipla da imagem
          }
        }
        else if (enter_state == HIGH && x_1 < 6){ // TROCAR POR 6
         x_1++;
         y_1 = 0;

         //ALTERNATIVA 1:
        if ((x_1) < 2) {
            cursor_tempo[(x_1) - 1] = ' ';
            cursor_tempo[(x_1)] = '-';
            }
         if ((x_1) == 2){
            cursor_tempo[(x_1) + 1] = '-';
            cursor_tempo[(x_1) - 1] = ' '; 
         }
        if ((x_1) > 2 && (x_1) < 4){
          cursor_tempo[(x_1)] = ' ';
          cursor_tempo[(x_1)+1] = '-';
          }
        if ( (x_1) == 4){
          cursor_tempo[(x_1)] = ' ';
          cursor_tempo[(x_1)+2] = '-';
          
          }
        if ( (x_1) > 4){
          cursor_tempo[(x_1) + 1] = ' ';
          cursor_tempo[(x_1) + 2] = '-';
          }
         delay(200);
        }
        else if (set_state == HIGH && enter_state == HIGH){  //VERIFICAR ESTAS CONDIÇÕES
          x_1 = 0;
          }
      }
      cond_3 = false;
      u8x8.clearDisplay();
      u8x8.setFont( u8x8_font_torussansbold8_r);
      u8x8.setCursor(1, 0);
      u8x8.print("Finalizou !!!");
      u8x8.setCursor(0, 3);
      u8x8.print("Tempo escolhido: ");
      u8x8.setCursor(0, 5);
      u8x8.print(String(contador[0]) + String(contador[1]) + ':' + String(contador[2]) + String(contador[3]) + ':' + String(contador[4]) + String(contador[5]));
    }
    delay(2000);
    u8x8.clearDisplay();
    int h = (contador[0]*10) + contador[1];
    int m = (contador[2]*10) + contador[3];
    int s = (contador[4]*10) + contador[5];
    int h_1,m_1,s_1;
    int temp_total = (s*1000)+(m*60000)+(h*3600000);
    int temporizador = 0;
    
    while(temp_total > 0){
      digitalWrite(led_amarelo, LOW);
      if(millis()-temporizador >= 1000){
        temp_total = temp_total - 1000;
        temporizador = millis();
        digitalWrite(led_amarelo, HIGH);
        }
        h_1 = ((temp_total-(temp_total%3600000))/3600000)%60;
        m_1 = ((temp_total-(temp_total%60000))/60000)%60;
        s_1 = ((temp_total-(temp_total%1000))/1000)%60;
        u8x8.setFont( u8x8_font_torussansbold8_r);
        u8x8.setCursor(1, 0);
        u8x8.print("Lendo padrão...");
        u8x8.setCursor(0, 3);
        u8x8.print(String((h_1-(h_1%10))/10) + String(h_1 % 10) + ':' + String((m_1-(m_1%10))/10) + String((m_1 % 10)) + ':' + String((s_1-(s_1%10))/10) + String((s_1 % 10)));
        
        if (millis()> 4000){
          Serial.print("Esperando tempo para Leitura !");
          }
        read_sensor_normal();
        
      }
      digitalWrite(led_amarelo, LOW);
      u8x8.clearDisplay();
      u8x8.setFont( u8x8_font_torussansbold8_r);
      u8x8.setCursor(0, 0);
      u8x8.print("Leitura Terminou !!");
      delay(2000);
      u8x8.clearDisplay();
      
  }
  
void read_sensor_normal(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  
  AcX=Wire.read()<<8|Wire.read();  
  AcY=Wire.read()<<8|Wire.read();  
  AcZ=Wire.read()<<8|Wire.read();
  Tmp = Wire.read() << 8 | Wire.read(); //0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX = Wire.read() << 8 | Wire.read(); //0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); //0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); //0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  
  if ((AcX / 2048.0) > max_acx){
    max_acx = AcX / 2048.0;
    }
  if ((AcY / 2048.0) > max_acy){
    max_acy = AcY / 2048.0;
    }
  if ((AcZ/2048.0) > max_acz){
    max_acz = AcZ / 2048.0;
    }
  if ((AcX/ 2048.0) < min_acx){
    min_acx = AcX / 2048.0;
    }
  if ((AcY / 2048.0) < min_acy){
    min_acy = AcY / 2048.0;
    }
  if ((AcZ / 2048.0 / 2048.0) < min_acz){
    min_acz = AcZ / 2048.0;
    }
  // Linha para observar o erro de máximo qual o valor tá ficando nas variáveis
   Serial.print("AcX = "); Serial.print(AcX / 2048.0);
   Serial.print(" | AcY = "); Serial.print(AcY / 2048.0);
   Serial.print(" | AcZ = "); Serial.print(AcZ / 2048.0);Serial.print(":::");
   Serial.print(max_acx);Serial.print(";"); Serial.print(max_acy);Serial.print(";"); Serial.print(max_acz);
   Serial.print(":::");Serial.print(min_acx);Serial.print(";"); Serial.print(min_acy);Serial.print(";"); Serial.println(min_acz);
  
}

void normal(){
  //VALORES POSITIVOS
      if(abs(0.85 * min_acx) <= abs(AcX / 2048.0) <= abs(1.15*max_acx)){
        indica_normal = 1;
        indica_disturbio1 = 0;
        digitalWrite(led_vermelho,LOW);

    }


      if( abs(0.85*min_acy) <= abs(AcY / 2048.0)<= abs(1.15*max_acy)){
          indica_normal = 1;
          indica_disturbio1 = 0;
          digitalWrite(led_vermelho, LOW);
    }

      if( abs(0.85*min_acz) <= abs(AcZ / 2048.0)<= abs(1.15*max_acz)){
          indica_normal = 1;
          indica_disturbio1 = 0;
          digitalWrite(led_vermelho, LOW);
    }

  }
void disturbio_1(){
  //MARGEM POSITIVA DA BORDA SUPERIOR
  if((AcX / 2048.0) > 0){
    if ((AcX / 2048.0) > (1.5*max_acx)){
      indica_normal = 0;
      indica_disturbio1 = 1;
      digitalWrite(led_vermelho, HIGH);
    }
    }
  if((AcY / 2048.0) > 0){
    if ((AcY / 2048.0) > (1.5*max_acx)){
      indica_normal = 0;
      indica_disturbio1 = 1;
      digitalWrite(led_vermelho, HIGH);
    }
    }
  if((AcZ / 2048.0) > 0){
    if ((AcZ / 2048.0) > (1.5*max_acz)){
      indica_normal = 0;
      indica_disturbio1 = 1;
      digitalWrite(led_vermelho, HIGH);
      } 
    }
  }

void init_MPU(){
  //MPU Initializing & Reset
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  //MPU6050 Clock Type
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0x03);     // Selection Clock 'PLL with Z axis gyroscope reference'
  Wire.endTransmission(true);

  //CONFIGURAÇÕES DO GIROSCÓPIO
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x1B);  // Gyroscope Configuration register
  //Wire.write(0x00);     // FS_SEL=0, Full Scale Range = +/- 250 [degree/sec]
  Wire.write(0x08);     // FS_SEL=1, Full Scale Range = +/- 500 [degree/sec]
  //Wire.write(0x10);     // FS_SEL=2, Full Scale Range = +/- 1000 [degree/sec]
  //Wire.write(0x18);     // FS_SEL=3, Full Scale Range = +/- 2000 [degree/sec]
  Wire.endTransmission(true);

  //ESCALA ACELEROMETRO
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x1C);  // Accelerometer Configuration register
  //Wire.write(0x00);     // AFS_SEL=0, Full Scale Range = +/- 2 [g]
  //Wire.write(0x08);     // AFS_SEL=1, Full Scale Range = +/- 4 [g]
  //Wire.write(0x10);     // AFS_SEL=2, Full Scale Range = +/- 8 [g]
  Wire.write(0x18);     // AFS_SEL=3, Full Scale Range = +/- 16 [g]
  Wire.endTransmission(true);

  //FILTRO BAIXA BAIXA
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x1A);  // DLPF_CFG register
  //Wire.write(0x00);     // Accel BW 260Hz, Delay 0ms / Gyro BW 256Hz, Delay 0.98ms, Fs 8KHz 
  //Wire.write(0x01);     // Accel BW 184Hz, Delay 2ms / Gyro BW 188Hz, Delay 1.9ms, Fs 1KHz 
  //Wire.write(0x02);     // Accel BW 94Hz, Delay 3ms / Gyro BW 98Hz, Delay 2.8ms, Fs 1KHz 
  //Wire.write(0x03);     // Accel BW 44Hz, Delay 4.9ms / Gyro BW 42Hz, Delay 4.8ms, Fs 1KHz 
  //Wire.write(0x04);     // Accel BW 21Hz, Delay 8.5ms / Gyro BW 20Hz, Delay 8.3ms, Fs 1KHz 
  //Wire.write(0x05);     // Accel BW 10Hz, Delay 13.8ms / Gyro BW 10Hz, Delay 13.4ms, Fs 1KHz 
  Wire.write(0x06);     // Accel BW 5Hz, Delay 19ms / Gyro BW 5Hz, Delay 18.6ms, Fs 1KHz 
  Wire.endTransmission(true);
}

void read_sensor(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  
  AcX=Wire.read()<<8|Wire.read();  
  AcY=Wire.read()<<8|Wire.read();  
  AcZ=Wire.read()<<8|Wire.read();
  Tmp = Wire.read() << 8 | Wire.read(); //0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX = Wire.read() << 8 | Wire.read(); //0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); //0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); //0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  normal();
  disturbio_1();
  Serial.print("AcX = "); Serial.print(abs(AcX / 2048.0));
  Serial.print(" | AcY = "); Serial.print(AcY / 2048.0);
  Serial.print(" | AcZ = "); Serial.print(AcZ / 2048.0);Serial.print(":::");
  Serial.println("id = ");Serial.print(("S"+String(id_equip[1])+"-"+String(id_equip[2])+String(id_equip[3])+String(id_equip[4])+String(id_equip[5])+"-"+String(id_equip[6])+String(id_equip[7])+String(id_equip[8])));

}

void loraSentPacket_data(){
  //beginPacket : abre um pacote para adicionarmos os dados para envio
  LoRa.beginPacket();
  //print: adiciona os dados no pacote
  LoRa.print((AcX / 2048.0));LoRa.print("|");LoRa.print((AcY / 2048.0));
  LoRa.print("|");LoRa.print((AcZ / 2048.0));LoRa.print("|");
  LoRa.print(indica_normal);LoRa.print("|");LoRa.print(indica_disturbio1);LoRa.print("|");
  LoRa.print(("S"+String(id_equip[1])+"-"+String(id_equip[2])+String(id_equip[3])+String(id_equip[4])+String(id_equip[5])+"-"+String(id_equip[6])+String(id_equip[7])+String(id_equip[8])));LoRa.print("|");
  //endPacket : fecha o pacote e envia
  LoRa.endPacket(); //retorno= 1:sucesso | 0: falha 
  }


void mostra_dados(){
  Serial.print("AcX = "); Serial.print(AcX);
  Serial.print(" | AcY = "); Serial.print(AcY);
  Serial.print(" | AcZ = "); Serial.println(AcZ);
  delay(10);
  
  }

void loraInit(){
  SPI.begin(SCK,MISO,MOSI,SS); //inicia a comunicação serial com o Lora
  LoRa.setPins(SS,RST,DI00); //configura os pinos que serão utlizados pela biblioteca (deve ser chamado antes do LoRa.begin)
  Wire.begin();
  init_MPU();
  //inicializa o Lora com a frequencia específica.
  if (!LoRa.begin(BAND))
  {
    Serial.print("Comunicação Lora falhou !");
    while (1);
  }
  //indica no display que inicilizou corretamente.
  Serial.print("Comunicação Lora ok !");
  delay(1000);

  }
 void calibraSensor(){
  u8x8.setFont( u8x8_font_torussansbold8_r);
  u8x8.drawString(0,4, "Calibrando ");
   u8x8.drawString(0,5, "Sensor...");
  while(cont_calibra < 151){
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);  
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr,14,true);  
    AcX=Wire.read()<<8|Wire.read();  
    AcY=Wire.read()<<8|Wire.read();  
    AcZ=Wire.read()<<8|Wire.read();
    Tmp = Wire.read() << 8 | Wire.read(); //0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    GyX = Wire.read() << 8 | Wire.read(); //0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    GyY = Wire.read() << 8 | Wire.read(); //0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    GyZ = Wire.read() << 8 | Wire.read(); //0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
    cal_acx[cont_calibra] = AcX / 2048.0;
    cal_acy[cont_calibra] = AcY / 2048.0;
    cal_acz[cont_calibra] = AcZ / 2048.0;
    cont_calibra ++;
    }

    max_acx = cal_acx[140];
    min_acx = cal_acx[140];
    max_acy = cal_acy[140];
    min_acy = cal_acy[140];
    max_acz = cal_acz[140];
    min_acz = cal_acz[140];
    u8x8.clearDisplay();
  }

void pisca_led(){
  led.blink(8000, 400);
  
}

    
