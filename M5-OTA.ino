
#include <WiFiClient.h>
#include <ESP_WiFiManager.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include <M5StickCPlus.h>
#include <HardwareSerial.h>
#include <EEPROM.h>
#include <WebServer.h>
#include <Update.h>
#include <ESPmDNS.h>
#include <HTTPUpdate.h>
#include <SPIFFS.h>

int modoadmin;

String okStr = "ok";
//imagem
#include "logo_limelocker.h"
// fim da imagem

const char* host = "esp32";

WebServer server(80);

int caso;

//váriaveis para configurar o nome do device
bool shouldSaveConfig = false;
char device[60];
String deviceStr;
String deviceCadastrado;//usar esse
String _device;

//váriaveis para o username
char username[60];
String usernameStr;
String usernameCadastrado;//usar esse
String _username;

//váriaveis para a senha
char senha[60];
String senhaStr;
String senhaCadastrado;//usar esse
String _senha;

String Data;


/*Configuração do servidor mqtt*/
const char* mqtt_server = "157.245.223.123";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
byte screenState = 0;
const int serverPort = 80;
const int timerInterval = 30000;    // time between each HTTP POST image
unsigned long previousMillis = 0;   // last time image was sent
HardwareSerial SerialA(2);
RTC_TimeTypeDef RTC_TimeStruct;
String messageID;
String messageTemp;
String messageWishlist;
int messageDenied;
int r;
int i;
int w;
int d;
#define TRIG 0
#define LED 10

int conectado;
bool initialConfig = false;
#define USE_DHCP_IP     true

//SSID and PW for cofig portal
//String ssid = "ESP_" + String(ESP_getChipId(), HEX);
String ssid = "M5-Scanner";
const char* password = "password";

// SSID and PW for your Router
String Router_SSID;
String Router_Pass;

// Use DHCP
IPAddress stationIP   = IPAddress(0, 0, 0, 0);
IPAddress gatewayIP   = IPAddress(192, 168, 2, 1);
IPAddress netMask     = IPAddress(255, 255, 255, 0);
IPAddress dns1IP      = gatewayIP;
IPAddress dns2IP      = IPAddress(8, 8, 8, 8);

RTC_TimeTypeDef TimeStruct;

/*Login page*/

const char* loginIndex =
  "<form name='loginForm'>"
  "<table width='20%' bgcolor='A09F9F' align='center'>"
  "<tr>"
  "<td colspan=2>"
  "<center><font size=4><b>ESP32 Login Page</b></font></center>"
  "<br>"
  "</td>"
  "<br>"
  "<br>"
  "</tr>"
  "<td>Username:</td>"
  "<td><input type='text' size=25 name='userid'><br></td>"
  "</tr>"
  "<br>"
  "<br>"
  "<tr>"
  "<td>Password:</td>"
  "<td><input type='Password' size=25 name='pwd'><br></td>"
  "<br>"
  "<br>"
  "</tr>"
  "<tr>"
  "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
  "</tr>"
  "</table>"
  "</form>"
  "<script>"
  "function check(form)"
  "{"
  "if(form.userid.value=='Limelocker' && form.pwd.value=='admin')"
  "{"
  "window.open('/serverIndex')"
  "}"
  "else"
  "{"
  " alert('Error Password or Username')/*displays error message*/"
  "}"
  "}"
  "</script>";

/*Server Index Page*/

const char* serverIndex =
  "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
  "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
  "<input type='file' name='update'>"
  "<input type='submit' value='Update'>"
  "</form>"
  "<div id='prg'>progress: 0%</div>"
  "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')"
  "},"
  "error: function (a, b, c) {"
  "}"
  "});"
  "});"
  "</script>";

void gravarModoadmin(int numero) {
  EEPROM.write(2, numero);
  EEPROM.commit();
}

void gravarDevice(char d, String name) {
  int size = name.length();
  for (int i = 0; i < size; i++) {
    EEPROM.write(d + i, name[i]);
  }
  EEPROM.write(d + size, '\0');
  EEPROM.commit();
}

void gravarUsername(char r, String username2) {
  int size = username2.length();
  for (int i = 0; i < size; i++) {
    EEPROM.write(r + i, username2[i]);
  }
  EEPROM.write(r + size, '\0');
  EEPROM.commit();
}
void gravarSenha(char y, String senha2) {
  int size = senha2.length() + 1;
  for (int i = 0; i < size; i++) {
    EEPROM.write(y + i, senha2[i]);
  }
  EEPROM.write(y + size, '\0');
  EEPROM.commit();
}

String lerDevice(char ad) {
  int i;
  char da[100];
  int len = 0;
  unsigned char k;
  k = EEPROM.read(ad);
  while (k != '\0' && len < 500) {
    k = EEPROM.read(ad + len);
    da[len] = k;
    len++;
  }
  da[len] = '\0';
  return String(da);
}
String lerSenha(char ab) {
  int u;
  char bc[100];
  int tamanho = 0;
  unsigned char v;
  v = EEPROM.read(ab);
  while (v != '\0' && tamanho < 500) {
    v = EEPROM.read(ab + tamanho);
    bc[tamanho] = v;
    tamanho++;
  }
  bc[tamanho] = '\0';
  return String(bc);
}
String lerUsername(char ac) {
  int o;
  char ba[100];
  int tam = 0;
  unsigned char p;
  p = EEPROM.read(ac);
  while (p != '\0' && tam < 500) {
    p = EEPROM.read(ac + tam);
    ba[tam] = p;
    tam++;
  }
  ba[tam] = '\0';
  return String(ba);
}

void SaveConfigCallback() {
  shouldSaveConfig = true;
}

void connectWifi() {
  Serial.println("\nStarting ConfigOnSwitch on " + String(ARDUINO_BOARD));
  unsigned long startedAt = millis();
  ESP_WiFiManager ESP_wifiManager("ConfigOnSwitch");
  ESP_wifiManager.setDebugOutput(true);

  // Use only to erase stored WiFi Credentials
  //resetSettings();
  //ESP_wifiManager.resetSettings();

  ESP_wifiManager.setMinimumSignalQuality(-1);
  Router_SSID = ESP_wifiManager.WiFi_SSID();
  Router_Pass = ESP_wifiManager.WiFi_Pass();
  
  //Remove this line if you do not want to see WiFi password printed
  Serial.println("Stored: SSID = " + Router_SSID + ", Pass = " + Router_Pass);

  // SSID to uppercase
  ssid.toUpperCase();

  if (Router_SSID == "") {
    Serial.println("Open Config Portal without Timeout: No stored Credentials.");
    //it starts an access point
    //and goes into a blocking loop awaiting configuration
    if (!ESP_wifiManager.startConfigPortal((const char *) ssid.c_str(), password))
      Serial.println("Not connected to WiFi but continuing anyway.");
    else
      Serial.println("WiFi connected...yeey :)");
  }

#define WIFI_CONNECT_TIMEOUT        30000L
#define WHILE_LOOP_DELAY            200L
#define WHILE_LOOP_STEPS            (WIFI_CONNECT_TIMEOUT / ( 3 * WHILE_LOOP_DELAY ))

  startedAt = millis();

  while ( (WiFi.status() != WL_CONNECTED) && (millis() - startedAt < WIFI_CONNECT_TIMEOUT )) {
    WiFi.mode(WIFI_STA);
    WiFi.persistent (true);
    // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(Router_SSID);
    WiFi.config(stationIP, gatewayIP, netMask);
    //WiFi.config(stationIP, gatewayIP, netMask, dns1IP, dns2IP);
    WiFi.begin(Router_SSID.c_str(), Router_Pass.c_str());
    int i = 0;
    while ((!WiFi.status() || WiFi.status() >= WL_DISCONNECTED) && i++ < WHILE_LOOP_STEPS) {
      delay(WHILE_LOOP_DELAY);
    }
  }
  Serial.print("After waiting ");
  Serial.print((millis() - startedAt) / 1000);
  Serial.print(" secs more in setup(), connection result is ");
  if (WiFi.status() == WL_CONNECTED) {
    conectado = 1;
    Serial.print("connected. Local IP: ");
    Serial.println(WiFi.localIP());
  } else
    conectado = 0;
  Serial.println(ESP_wifiManager.getStatus(WiFi.status()));
}

int dadoCompleto = 0;


void setup () {
  Serial.begin(115200);
  SerialA.begin(9600, SERIAL_8N1, 26, 36);
  M5.begin();
  EEPROM.begin(512);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.println(WiFi.softAPIP());
  connectWifi();
  Serial.println("\nIniciando setup");
  RTC_TimeTypeDef TimeStruct;
  TimeStruct.Minutes = 0;
  TimeStruct.Seconds = 0;
  M5.Rtc.SetTime(&TimeStruct);
  M5.Rtc.GetTime(&RTC_TimeStruct);

  deviceCadastrado = lerDevice(3);
  usernameCadastrado = lerUsername(16);
  senhaCadastrado = lerSenha(61);
  Serial.println("Informações Gravadas: ");
  Serial.println(deviceCadastrado);
  Serial.println(usernameCadastrado);
  Serial.println(senhaCadastrado);
  Data = deviceCadastrado + "//$//" + usernameCadastrado + "//$//" + senhaCadastrado + 1;
  Serial.println("Soma das Strings:");
  Serial.println(Data);
  modoadmin = EEPROM.read(2);
  Serial.print("modoadmi: ");
  Serial.println(modoadmin);
  if (modoadmin == 1) {
    Serial.print("modo admin: ");
    Serial.println(modoadmin);
    Serial.println("Abrindo modo de configuração AP");
    check_status();
  }

  pinMode(TRIG, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(TRIG, LOW);
  digitalWrite(LED, LOW);

  M5.Axp.ScreenBreath(11);
  M5.Lcd.setRotation(1);
  M5.Lcd.setTextFont(8);
  M5.Lcd.setCursor(10, 10, 4);
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setTextSize(1);
  if (WiFi.status() == WL_CONNECTED) {
    caso = 6;
    OTA();
  }
  if (WiFi.status() != WL_CONNECTED) {
    caso = 7;
  }
  /*if (deviceCadastrado == 0 or usernameCadastrado == 0 or senhaCadastrado == 0){
    caso = 8;
    }*/
}
char produtoArray[0];

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  if (String(topic) == "esp32/id_return/") {
    for (r = 0; r < length ; r++) {//lenght + 1
      Serial.print ((char)message[r]);
      messageID += (char)message[r];
      Serial.println("message ID");
      Serial.println (messageID);
    }
  }

  if (String(topic) == "esp32/wishlist_return/") {
    for (w = 0; w < length ; w++) {//lenght + 1
      Serial.print ((char)message[w]);
      messageWishlist += (char)message[w];
      Serial.println("messageWishlist:");
      Serial.println (messageWishlist);
      Serial.println(messageWishlist.substring(2));
    }
  }
  if (String(topic) == "esp32/id_denied/") {
    for (d = 0; d < length ; d++) {//lenght + 1
      Serial.print ((char)message[d]);
      messageDenied += (char)message[d];
      Serial.println(" if id_denied / messageDenied:");
      Serial.println (messageDenied);
    }
    if (messageDenied != 0) {
      Serial.println("Dados incorretos");
      padrao();
      M5.Lcd.println("Dados incorretos");
      M5.Lcd.println("Abrindo modo de");
      M5.Lcd.println("configuração");
      M5.Lcd.println("Atualize os dados");
      delay(3000);
      gravarModoadmin(1);
      ESP.restart();
    }
  }
  if (String(topic) == "esp32/product_return/") {
    for ( i = 0; i < length; i++) {
      Serial.println((char)message[i]);
      if ((char)message[i] == 'Ç') {
        (char)message[i] == 'C';
      }
      messageTemp += (char)message[i];
    }
  }
  RTC_TimeTypeDef TimeStruct;
  TimeStruct.Minutes = 0;
  M5.Rtc.SetTime(&TimeStruct);
  caso = 3;
  padrao();
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("M5-Scanner", "sammy", "Lime#1234")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/id_return/");
      client.subscribe("esp32/product_return/");
      client.subscribe("esp32/wishlist_return/");
      client.subscribe("esp32/id_denied/");
      //      char dataArray [Data.length() + 2];
      //      Data.toCharArray(dataArray, Data.length());
      //      Serial.println("Data in Array");
      //      Serial.println((const char*) dataArray);
      //      client.publish("esp32/data_client/device1234a", dataArray);

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void heartBeatPrint(void) {
  static int num = 1;

  if (WiFi.status() == WL_CONNECTED)
    Serial.print("H");// H means connected to WiFi
  else
    Serial.print("F");// F means not connected to WiFi

  if (num == 80) {
    Serial.println();
    num = 1;
  }
  else if (num++ % 10 == 0) {
    Serial.print(" ");
  }
}

void check_status() {
  Serial.println("Entrou no check status");
  // is configuration portal requested?
  Serial.println("\nConfiguration portal requested.");
  ESP_WiFiManager ESP_wifiManager;
  ESP_wifiManager.setSaveConfigCallback(SaveConfigCallback);
  Serial.print("Opening configuration portal. ");
  Router_SSID = ESP_wifiManager.WiFi_SSID();
  if (Router_SSID != "") {
    ESP_wifiManager.setConfigPortalTimeout(240);
    Serial.println("Got stored Credentials. Timeout 240s");
  } else
    Serial.println("No stored Credentials. No timeout");

  ESP_WMParameter custom_device("Nome do dispositivo", "Nome de dispositivo(Max 11 caracteres)", device, 12);
  ESP_wifiManager.addParameter(&custom_device);

  ESP_WMParameter custom_username("Usuário do app Limelocker", "Usuario do app Limelocker", username, 60);
  ESP_wifiManager.addParameter(&custom_username);

  ESP_WMParameter custom_senha("Senha do app Limelocker", "Senha do app Limelocker", senha, 60);
  ESP_wifiManager.addParameter(&custom_senha);

  ESP_wifiManager.setMinimumSignalQuality(-1);
  ESP_wifiManager.setSTAStaticIPConfig(stationIP, gatewayIP, netMask, dns1IP, dns2IP);
  if (!ESP_wifiManager.startConfigPortal((const char *) ssid.c_str(), password)) {
    Serial.println("Not connected to WiFi but continuing anyway.");
    padrao();
    M5.Lcd.println("Erro na conexao");
    M5.Lcd.println("Por favor,");
    M5.Lcd.println("Espere um momento");
    delay(3000);
    padrao();
    M5.Lcd.println("Reiniciando o ");
    M5.Lcd.println("dispositivo para");
    M5.Lcd.println("tentar a conexao");
    Serial.println("Reiniciando o M5");
    ESP.restart();
  } else {
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
    caso = 0;
    delay(3000);
  }
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  strcpy(device, custom_device.getValue());
  Serial.println("nome Device:");
  Serial.println((const char*)device);
  _device = String (device);
  Serial.println("nome Device String:");
  Serial.println(_device);
  gravarDevice(3, _device);

  strcpy(username, custom_username.getValue());
  Serial.println("Username:");
  Serial.println((const char*)username);
  _username = String (username);
  Serial.println("Username String:");
  Serial.println(_username);
  gravarUsername(16, _username);

  strcpy(senha, custom_senha.getValue());
  Serial.println("Senha:");
  Serial.println((const char*)senha);
  _senha = String (senha);
  Serial.println("Sennha String:");
  Serial.println(_senha);
  gravarSenha(61, _senha);

  static ulong checkstatus_timeout = 0;
#define HEARTBEAT_INTERVAL    10000L
  // Print hearbeat every HEARTBEAT_INTERVAL (10) seconds.
  if ((millis() > checkstatus_timeout) || (checkstatus_timeout == 0)) {
    heartBeatPrint();
    checkstatus_timeout = millis() + HEARTBEAT_INTERVAL;
  }
  //caso = 4;
  messageDenied = 0;
  gravarModoadmin(0);
  ESP.restart();
}
void padrao() {
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setCursor(10, 10, 4);
}

void telaInicial() {
      digitalWrite(TRIG, LOW); // Desliga o leitor
    //digitalWrite(LED, HIGH);
  int x = random(M5.Lcd.width()  - logo_limelocker_width);
  int y = random(M5.Lcd.height() - logo_limelocker_height);
  M5.Lcd.drawXBitmap(x, y, logo_limelocker_bits, logo_limelocker_width, logo_limelocker_height, TFT_GREEN);
  M5.Rtc.GetTime(&RTC_TimeStruct);
  if ( WiFi.status() != WL_CONNECTED) {
    padrao();
    M5.Lcd.println("Sem internet, acesse");
    M5.Lcd.println("a rede M5-Scanner");
    M5.Lcd.println("senha: password");
    M5.Lcd.println("digite no navegador:");
    M5.Lcd.println("192.168.4.1");
    RTC_TimeTypeDef TimeStruct;
    TimeStruct.Minutes = 0;
    TimeStruct.Seconds = 0;
    M5.Rtc.SetTime(&TimeStruct);
    gravarModoadmin(1);
    ESP.restart();
  }
  if (M5.BtnB.isPressed() and RTC_TimeStruct.Seconds == 10 ) {
    padrao();
    M5.Lcd.println("Acesse a rede:");
    M5.Lcd.println("M5-Scanner");
    M5.Lcd.println("senha: password");
    M5.Lcd.println("digite no navegador:");
    M5.Lcd.println("192.168.4.1");
    RTC_TimeTypeDef TimeStruct;
    TimeStruct.Minutes = 0;
    TimeStruct.Seconds = 0;
    M5.Rtc.SetTime(&TimeStruct);
    gravarModoadmin(1);
    ESP.restart();
  }

  if (digitalRead(37) == LOW) {
    Serial.println("Botao A pressionado, vai para a tela de leitura do QR");
    digitalWrite(TRIG, HIGH);
    padrao();
    M5.Lcd.println("Leia o produto");
    //M5.Lcd.fillScreen(TFT_WHITE);
    RTC_TimeTypeDef TimeStruct;
    TimeStruct.Minutes = 0;
    TimeStruct.Seconds = 0;
    M5.Rtc.SetTime(&TimeStruct);
    caso = 2;
    padrao();
  }
  /*if ( RTC_TimeStruct.Minutes == 2) {
    M5.Axp.PowerOff();
    RTC_TimeTypeDef TimeStruct;
    TimeStruct.Minutes = 0;
    TimeStruct.Seconds = 0;
    M5.Rtc.SetTime(&TimeStruct);
    }*/
}

void leitorCodigo() {
  M5.Rtc.GetTime(&RTC_TimeStruct);
  M5.Lcd.setCursor(0, 0, 4);
  M5.Lcd.println("Leia o Codigo");
  M5.Lcd.println("do produto");

  if (screenState == 0) {
    // Home => Turn Scanner ON/OFF
    if (digitalRead(37) == LOW) {
      digitalWrite(TRIG, LOW);
    } else {
      digitalWrite(TRIG, HIGH);
    }
  }
  if (SerialA.available() > 0) {
    digitalWrite(TRIG, LOW); // Desliga o leitor
    digitalWrite(LED, LOW);
    //M5.Beep.tone(4000);
    String tempStr;
    while (SerialA.available() > 0) {
      char ch = SerialA.read();
      tempStr += ch;
      Serial.print(ch);
    }
    delay(300);
    //M5.Beep.mute();
    digitalWrite(LED, HIGH);
    Serial.println("");
    Serial.print("code read: ");
    Serial.println(tempStr);
    // Convert the value to a char array
    char tempCode[tempStr.length() + 1];
    tempStr.toCharArray(tempCode, tempStr.length());
    Serial.print("temp code read: ");
    Serial.println(tempCode);
    client.publish("esp32/product_code/device1234a", tempCode);
    //Should start timer do wait for response
  }
  if (RTC_TimeStruct.Seconds == 45) {
    RTC_TimeTypeDef TimeStruct;
    TimeStruct.Minutes = 0;
    TimeStruct.Seconds = 0;
    M5.Rtc.SetTime(&TimeStruct);
    caso = 1;
    padrao();
    digitalWrite(TRIG, LOW);
    digitalWrite(LED, LOW);
  }
}
int botaoPress;
void quantidade() {
  //padrao();
  M5.Rtc.GetTime(&RTC_TimeStruct);
  M5.Lcd.setCursor(0, 0, 4);
  M5.Lcd.println("Quantidade");
  M5.Lcd.println("do produto para");
  M5.Lcd.println("adicionar:");
  M5.Lcd.println(botaoPress);
  if (M5.BtnA.wasPressed()) {
    botaoPress++;
    Serial.println(botaoPress);
    padrao();
  }
  if (M5.BtnB.wasPressed() && botaoPress == 0) {
    padrao();
    M5.Lcd.println("Nada Adicionado");
    delay(2000);
    padrao();
    caso = 1;
    messageTemp = "";
    messageID = "";
    messageWishlist = "";
    RTC_TimeTypeDef TimeStruct;
    TimeStruct.Minutes = 0;
    M5.Rtc.SetTime(&TimeStruct);
    botaoPress = 0;
  }
  if (M5.BtnB.wasPressed() && botaoPress != 0) {
    //    botaoPress += 1;
    for (botaoPress == 0; botaoPress--;) {
      Serial.print("Adicionando o produto na wishlist, quantidade: ");
      Serial.println(botaoPress);
      padrao();
      M5.Lcd.println("Adicionando");
      M5.Lcd.println("produtos na");
      M5.Lcd.println("lista de desejo,");
      M5.Lcd.println("aguarde");
      messageWishlist = "";
      String msgWishlist;
      msgWishlist = messageID  + deviceCadastrado + 2;
      char messageArray [msgWishlist.length() + 1];
      msgWishlist.toCharArray(messageArray, msgWishlist.length());
      client.publish("esp32/wishlist/device1234a", messageArray);
      delay(500);
    }
    padrao();
    M5.Lcd.println("Produto adicionado");
    M5.Lcd.println("na lista");
    M5.Lcd.println("de desejos");
    delay(2000);
    Serial.println("botaoPress depois de enviado");
    Serial.println(botaoPress);
    padrao();
    caso = 1;
    messageTemp = "";
    messageID = "";
    //msgWishlist = "";
    messageWishlist = "";
    RTC_TimeTypeDef TimeStruct;
    TimeStruct.Minutes = 0;
    M5.Rtc.SetTime(&TimeStruct);
    botaoPress = 0;
  }
  if (RTC_TimeStruct.Minutes == 1) {
    padrao();
    M5.Lcd.println("Tempo expirado");
    delay(2000);
    padrao();
    caso = 1;
    messageTemp = "";
    messageID = "";
    messageWishlist = "";
    RTC_TimeTypeDef TimeStruct;
    TimeStruct.Minutes = 0;
    M5.Rtc.SetTime(&TimeStruct);
    botaoPress = 0;
  }
}

void telaProduto() {
  M5.Lcd.setCursor(10, 10, 4);
  //M5.Lcd.println((const char*)produtoArray);
  M5.Lcd.println(messageTemp);
  M5.Rtc.GetTime(&RTC_TimeStruct);
  if (M5.BtnA.wasPressed()) {
    caso = 5;
    quantidade();
    padrao();
    RTC_TimeTypeDef TimeStruct;
    TimeStruct.Minutes = 0;
    TimeStruct.Seconds = 0;
    M5.Rtc.SetTime(&TimeStruct);
    botaoPress = 0;
  }
  if (RTC_TimeStruct.Minutes == 1) {
    padrao();
    M5.Lcd.println("Tempo expirado");
    delay(2000);
    padrao();
    caso = 1;
    messageTemp = "";
    messageID = "";
    RTC_TimeTypeDef TimeStruct;
    TimeStruct.Minutes = 0;
    M5.Rtc.SetTime(&TimeStruct);
  }
}
//**************************************************************************************************************************************
void wishlist() {
  Serial.println("AINDA NÃO IMPLEMENTADO");
  if ( messageWishlist == okStr ) {
    Serial.println("substring:");
    Serial.println(messageWishlist.substring(2));
    padrao();
    M5.Lcd.println("Produto adicionado");
    M5.Lcd.println("na wishlist");
    delay(2000);
    padrao();
    caso = 1;
    messageTemp = "";
    messageID = "";
    RTC_TimeTypeDef TimeStruct;
    TimeStruct.Minutes = 0;
    M5.Rtc.SetTime(&TimeStruct);
  }
  if (messageWishlist.substring(2) != okStr) {
    Serial.println("substring:");
    Serial.println(messageWishlist.substring(2));
    M5.Lcd.println("Falhou!");
    M5.Lcd.println("Produto nao adicionado");
    M5.Lcd.println("na wishlist");
    delay(3000);
    messageTemp = "";
    messageID = "";
    RTC_TimeTypeDef TimeStruct;
    TimeStruct.Minutes = 0;
    M5.Rtc.SetTime(&TimeStruct);
    caso = 1;
    padrao();
  }
}

void conexaoWifi() {
  if (WiFi.status() == WL_CONNECTED) {
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.setCursor(10, 10, 4);
    M5.Lcd.println("Conectado na rede");
    M5.Lcd.println(Router_SSID);
    delay(3000);
    padrao();
    caso = 1;
  }
  if (WiFi.status() != WL_CONNECTED) {
    padrao();
    M5.Lcd.println("Sem internet, acesse");
    M5.Lcd.println("a rede M5-Scanner");
    M5.Lcd.println("senha: password");
    M5.Lcd.println("digite no navegador:");
    M5.Lcd.println("192.168.4.1");
    delay(3000);
    gravarModoadmin(1);
    ESP.restart();
  }
}

void dadosCadastrados() {
  char deviceCadastradoArray [deviceCadastrado.length() + 1];
  deviceCadastrado.toCharArray(deviceCadastradoArray, deviceCadastrado.length());

  Serial.println("Enviando dados de login");
  char dataArray [Data.length() + 2];
  Data.toCharArray(dataArray, Data.length());
  Serial.println("Data in Array");
  Serial.println((const char*) dataArray);
  client.publish("esp32/data_client/device1234a", dataArray);
  Serial.println("Dados enviados");
  Serial.print("void setup() / messageDenied:");
  Serial.println(messageDenied);
  padrao();
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setCursor(0, 0, 4);
  M5.Lcd.println("Iniciando:");//iniciando
  Serial.println((const char*)deviceCadastradoArray);
  M5.Lcd.println("Conectado na rede");
  M5.Lcd.println(Router_SSID);
  delay(3000);
  padrao();
  M5.Lcd.println("Nome:");
  M5.Lcd.println(deviceCadastrado);
  M5.Lcd.println("Usuario:");
  M5.Lcd.println(usernameCadastrado);
  delay(3000);
  padrao();
  RTC_TimeTypeDef TimeStruct;
  TimeStruct.Minutes = 0;
  TimeStruct.Seconds = 0;
  M5.Rtc.SetTime(&TimeStruct);
  caso = 1;
}

void semInternet() {
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setCursor(0, 0, 4);
  M5.Lcd.println("Sem internet, acesse");
  M5.Lcd.println("a rede M5-Scanner");
  M5.Lcd.println("senha: password");
  M5.Lcd.println("digite no navegador:");
  M5.Lcd.println("192.168.4.1");
  delay(3000);
  gravarModoadmin(1);
  ESP.restart();
}

void dadosVazios() {
  padrao();
  M5.Lcd.println("Dados incompletos");
  M5.Lcd.println("cadastre os dados");
  M5.Lcd.println("para continuar");
  M5.Lcd.println("com a operacao");
  delay(3000);
  padrao();
  M5.Lcd.println("Acesse a rede:");
  M5.Lcd.println("M5-Scanner");
  M5.Lcd.println("senha: password");
  M5.Lcd.println("digite no navegador:");
  M5.Lcd.println("192.168.4.1");
  delay(4000);
  gravarModoadmin(1);
  ESP.restart();
}

void loop() {
  server.handleClient();
  M5.update();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  switch (caso) {
    case 0:
      conexaoWifi();
      break;
    case 1:
      telaInicial();
      break;
    case 2:
      leitorCodigo();
      break;
    case 3:
      telaProduto();
      break;
    case 4:
      padrao();
      M5.Lcd.println("Enviando dados");
      M5.Lcd.println("para o servidor");
      delay(2000);
      padrao();
      M5.Lcd.println("Reiniciando o ");
      M5.Lcd.println("dispositivo para");
      M5.Lcd.println("cadastro do dados");
      Serial.println("Reiniciando o M5");
      caso = 0;
      Serial.println("Reiniciando o M5 para cadastrar os dados");
      ESP.restart();
      break;
    case 5:
      quantidade();
      break;
    case 6:
      dadosCadastrados();
      break;
    case 7:
      semInternet();
      break;
    case 8:
      dadosVazios();
      break;
  }
}

void streamTimeoutCallback(bool timeout) {
  if (timeout) {
    //Stream timeout occurred
    Serial.println("Stream timeout, resume streaming...");
  }
}

void OTA() {
  Serial.print("OTA Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  if (!MDNS.begin(host)) { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  //return index page which is stored in serverIndex
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  //handling uploading firmware file
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      //flashing firmware to ESP
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();
}
