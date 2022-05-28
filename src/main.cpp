#include <ArduinoOTA.h>
#include <WiFi.h>
#include <time.h>
#include <Espalexa.h>
Espalexa espalexa;
// WiFi設定(家)
const char *WIFI_SSID = "eoRT-1b3cdbf-g";
const char *WIFI_PASS = "330e5c2e64e339";
// Wifi設定(エンジニアハウス)
// const char *WIFI_SSID ="HG8045-56A3-bg";
// const char *WIFI_PASS ="bxhzy2sx";

//時間設定
const char *ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 32400;
const int   daylightOffset_sec = 0;

// モード選択
int mode = 0;

//RGBのピン
// const int redPin = 12;
// const int greenPin = 13;
// const int bluePin = 14;
// const int whitePin = ?;

//チャンネル定義
const int redChannel = 1;
const int greenChannel = 2;
const int blueChannel = 3;
// const int whiteCannnel = 4;

// RGB数値代入用
int r = 0;
int g = 0;
int b = 0;
int ri = 0;
int gi = 0;
int bi = 0;
// int w = 0;
String rr = " r=";
String gg = " g=";
String bb = " b=";

long i;
String str_s;
long long_s;
void firstLightChanged(uint8_t brightness);

//RGB関数リセット
void chengeLedColor(int red, int green, int blue);
void colorSetup(int red, int green, int blue);
// void whiteClolor(int white);
void printLocalTime();

void setup() {
  Serial.begin(115200);
  //LED_PWM出力チャネルをGPIOに割り当てる(チャンネル、周波数、PWMの範囲)
  ledcSetup(redChannel, 12800, 8);
  ledcSetup(greenChannel, 128000, 8);
  ledcSetup(blueChannel, 12800, 8);
  // ledcSetup(whiteCannnel, 12800, 8);
  //LED_PWM出力チャネルをGPIOに割り当てる(ピン番号、チャンネル)
  ledcAttachPin(12, redChannel);
  ledcAttachPin(13, greenChannel);
  ledcAttachPin(14, blueChannel);
  //ledcAttachPin(13, whiteCannnel);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Wi-Fi接続中");
  // WiFi接続を確認
  while (WiFi.status() != WL_CONNECTED)
  {
    chengeLedColor(0, 0, 1);
    Serial.print(".");
    delay(150);
    chengeLedColor(0, 0, 0);
    delay(150);
  }

//arduinoOTAの設定など
  ArduinoOTA.setHostname("desk_esp32");
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  // アレクサに追加
  espalexa.addDevice("机", firstLightChanged); 
  espalexa.begin();
   //時間情報取得
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  // printLocalTime();
}
// 時刻格納作成
struct tm timeInfo; //時刻を格納するオブジェクト
char s[20]; //文字列格納用

void loop() {
  ArduinoOTA.handle();
  getLocalTime(&timeInfo); //tmオブジェクトのtimeInfoに現在時刻を入れ込む
  sprintf(s, "%02d%02d", timeInfo.tm_hour, timeInfo.tm_min); //変換してsに入れてる？

  espalexa.loop();
  delay(1);
  chengeLedColor(r, g, b);
  if(mode == 1){
    switch (timeInfo.tm_hour){
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
        colorSetup(100, 100, 0);
        break;
      case 6:
        colorSetup(100, 100, 100);
        break;
      case 7:
        colorSetup(120, 120, 120);
        break;
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
      case 15:
      case 16:
        colorSetup(150, 150, 150);
        break;
      case 17:
        colorSetup(150, 150, 110);
        break;
      case 18:
        colorSetup(120, 120, 80);
        break;
      case 19:
        colorSetup(100,100,45);
        break;
      case 20: 
        colorSetup(100, 100, 15);
        break;
      case 21:
      case 22:
      case 23:
        colorSetup(100,100,0);
        break;
      default:
        break;
    } 
  }
  else{
    switch (timeInfo.tm_hour){
      case 6:
        if ((timeInfo.tm_min >= 0) && (timeInfo.tm_min <=15)){
            
          while(r <= 130){
            r++;
            g++;
            b++;
            chengeLedColor(r, g, b);
            delay(10);
          }
          while(r >= 1){
            r--;
            g--;
            b--;
            chengeLedColor(r, g, b);
            delay(10);
          }
          
        } 
    }
  }
}



void firstLightChanged(uint8_t brightness) {
  Serial.println(brightness);
  if (brightness == 255) {
    mode = 1;
    Serial.println(brightness);
  }
  else if(brightness == 27){
    mode = 0;
    for(i = 0; i <= 9; i++){
      r++;
      g++;
      b++;
      chengeLedColor(r, g, b);
      delay(50);
    }
  }
  else if(brightness == 53){
    mode = 0;
    for(i = 0; i <= 9; i++){
      r--;
      g--;
      b--;
      chengeLedColor(r, g, b);
      delay(50);
    }
  }
  else if(brightness == 78){
    mode = 0;
    colorSetup(200,0,0);
  }
  else  {
    mode = 0;
    while((r >= 1) || (g >= 1) || (b >= 1)){
      r--;
      g--;
      b--;
   
      r = max(0, r);
      g = max(0, g);
      b = max(0, b);

      chengeLedColor(r, g, b);
    
      delay(50);
      Serial.println(brightness);
    }
  }
}

void chengeLedColor(int red, int green, int blue){
  ledcWrite(redChannel, red);
  ledcWrite(greenChannel, green);
  ledcWrite(blueChannel, blue);
}
void colorSetup(int red, int green, int blue){
  while(true){
    if(r <= red - 1){
      r++;
    }
    else if(r >= red + 1){
      r--;
    }
    else{
      r = red;
    }

    if(g <= green -1 ){
      g++;
    }
    else if(g >= green + 1){
      g--;
    }
    else{
      g = green;
    }

    if(b <= blue - 1){
      b++;
    }
    else if(b >= blue + 1){
      b--;
    }
    else{
      b = blue;
    }

    if((r == red) && (g == green) && (b == blue)){
      break;
    }
    chengeLedColor(r, g, b);
    delay(50);
    // かくにんよう
    Serial.print("ri=");
    Serial.print(r);
    Serial.print(" gi=");
    Serial.print(g);
    Serial.print(" bi=");
    Serial.println(b);
  }
}
