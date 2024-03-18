#include <WiFi.h> //WiFiライブラリのインポート
#include <WiFiClientSecure.h>
#include <BlynkSimpleEsp32.h> //Blynkのライブラリのインポート

/*Blynk側のデバイス名、テンプレID、トークンの指定*/
#define BLYNK_TEMPLATE_ID "xxxxx"
#define BLYNK_DEVICE_NAME "xxxxx"
#define BLYNK_AUTH_TOKEN "xxxxx"

#define SoundSensor 32 //音センサーのピン番号指定
#define ledPin 25 //LEDピン番号指定
int SoundValue; //サウンドセンサーの値 
int ledStatus = 0; //LEDのON/OFFの値。0でOFF 1でON
// WiFi Setting
char auth[] = "xxxxx"; //Blynkのauthトークンの指定 
const char* ssid     = "xxxxx"; //Wifi用のSSIDとpass指定
const char* password = "xxxxx";

// LINE通知用Setting
const char* host = "notify-api.line.me";
const char* token = "xxxxx"; //Your API key
const char* message = "5秒以上の騒音が検知されました。";

//電源投入時の起動処理
void setup(){
  Serial.begin(115200); //起動時の周波数セットアップ
  pinMode(SoundSensor, INPUT); //音センサーのセットアップ
  ledcSetup(0, 12800, 8); //LEDのセットアップ
  ledcAttachPin(ledPin, 0); //LEDの利用するチャネルと結び付け
  Blynk.begin(auth, ssid, password); //Blynkのセットアップ
  WiFi.begin(ssid, password);//WiFi接続処理
}

//メインの処理
void loop(){
  Blynk.run(); //Blynkの起動処理
  SoundValue=analogRead(SoundSensor); //サウンドセンサーの音をSoundValueに代入
  //LEDが点灯時かつ、音量値が1000以上を5秒間観測した場合にLINEに通知が届く  
  if(SoundValue > 1000 && ledStatus == 1){
    delay(5000);
    if(SoundValue > 1000 && ledStatus == 1){
      send_line(); //条件を満たすとLINEに通知
    }
  }
  delay(25);
}
//LINE送信の処理
void send_line() {
  //Access to HTTPS (SSL communication)
  WiFiClientSecure client;
  //Required when connecting without verifying the server certificate
  client.setInsecure();

  Serial.println("Try");

  //SSL connection to Line's API server (port 443: https)
  if (!client.connect(host, 443)) {
    Serial.println("Connection failed");
    return;
  }
  Serial.println("Connected");

  // Send request
  String query = String("message=") + String(message);
  String request = String("") +
    "POST /api/notify HTTP/1.1\r\n" +
    "Host: " + host + "\r\n" +
    "Authorization: Bearer " + token + "\r\n" +
    "Content-Length: " + String(query.length()) +  "\r\n" + 
    "Content-Type: application/x-www-form-urlencoded\r\n\r\n" +
    query + "\r\n";
  client.print(request);
 
  // Wait until reception is complete
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }
  
  String line = client.readStringUntil('\n');
  Serial.println(line);
}

//Blynk V0ピンのON/OFF関数
BLYNK_WRITE(V0){
  ledStatus = param[0].asInt(); //Blynk側のスイッチでONを押すとledStatusにintの1を代入
  // digitalWrite(ledPin, ledStatus);
  ledcWrite(0,ledStatus);
}

