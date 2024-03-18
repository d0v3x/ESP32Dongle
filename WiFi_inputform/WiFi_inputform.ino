#include <WiFi.h>

const char *ssid = "ESP32-WiFi-5"; // アクセスポイントのSSID
const char *password = "esp32wifi"; // アクセスポイントのパスワード
const int serverPort = 80; // サーバーポート

WiFiServer server(serverPort); // WiFiサーバーオブジェクト

String ssidValue = ""; // 入力されたSSIDを保存する変数
String passwordValue = ""; // 入力されたパスワードを保存する変数

void setup() {
  Serial.begin(115200); // シリアル通信の開始
  
  // アクセスポイントの設定
  WiFi.softAP(ssid, password);
  delay(100);
  WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));

  // IPアドレスを取得し、サーバーを開始する
  IPAddress myIP = WiFi.softAPIP();
  server.begin();
}

void loop() {
  // クライアントからの接続を待機する
  WiFiClient client = server.available();
  if (client) { // クライアントが接続された場合
    Serial.println("New Client."); // シリアルモニターにメッセージを出力
    String currentLine = ""; // 空の文字列を作成
    
    // クライアントからのリクエストを待機する
    while (client.connected()) {
      if (client.available()) { // クライアントからのデータが利用可能な場合
        char c = client.read(); // 1文字読み取る
        Serial.write(c); // 読み取った文字をシリアルモニターに出力
        if (c == '\n') { // 改行文字が見つかった場合
          // 現在の行が空の場合（空行が見つかった場合）
          if (currentLine.length() == 0) {
            // ヘッダー部の終了を検出
            // クライアントにレスポンスを送信する
            sendHTML(client);
            
            // POSTデータを受け取る
            if (client.available()) {
              String postLine = client.readStringUntil('\r'); // データを読み取る
              Serial.println("Received POST data: " + postLine);
              
              // ユーザーが入力したSSIDとパスワードを取得する
              int ssidStartIndex = postLine.indexOf("ssid=") + 5;
              int ssidEndIndex = postLine.indexOf("&", ssidStartIndex);
              ssidValue = postLine.substring(ssidStartIndex, ssidEndIndex);
              int passwordStartIndex = postLine.indexOf("password=") + 9;
              passwordValue = postLine.substring(passwordStartIndex);
              
              // シリアルモニターにSSIDとパスワードを出力する
              Serial.println("SSID: " + ssidValue);
              Serial.println("Password: " + passwordValue);
            }
            break; // ループを抜ける
          } else { // 空行以外の場合
            // 新しい行の開始を検出
            currentLine = ""; // 現在の行をリセット
          }
        } else if (c != '\r') { // 改行または復帰文字以外の場合
          // 行のテキストを構築する
          currentLine += c;
        }
      }
    }
    // クライアントとの接続を閉じる
    client.stop();
    Serial.println("Client Disconnected."); // シリアルモニターにメッセージを出力
  }
}

void sendHTML(WiFiClient client) {
  // HTTPレスポンスヘッダを送信する
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  
  // HTMLコンテンツを送信する
  client.println("<!DOCTYPE html>");
  client.println("<html lang=\"ja\">");
  client.println("<head>");
  client.println("<meta charset=\"utf-8\">");
  client.println("<title>SSIDとパスワード入力フォーム</title>");
  client.println("</head>");
  client.println("<body>");
  client.println("<form id=\"wifiForm\" method=\"post\">");
  client.println("<p>SSID<br>");
  client.println("<input type=\"text\" id=\"ssid\" name=\"ssid\" size=\"15\"></p>");
  client.println("<p>パスワード<br>");
  client.println("<input type=\"text\" id=\"password\" name=\"password\" size=\"15\"></p>");
  client.println("<p><input type=\"submit\" value=\"送信\" id=\"submitButton\"></p>");
  client.println("</form>");
  client.println("</body>");
  client.println("</html>");
}
