// USE MODULE nodeMCU 1.0

// For loading URLs
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// For parsing json
#include "ArduinoJson.h"

// For Nokia 5110 LCD display
#include "Nokia_5110.h"

// WIFI
char* _wifi_ssid = "Frandsen International";
char* _wifi_password = "11111111";

int CLK = D4;
int DIN = D3;
int DC = D2;
int CE = D1;
int RST = D0;
Nokia_5110 lcd = Nokia_5110(RST, CE, DC, DIN, CLK);
// Nokia_LCD lcd(D4 /* CLK */, D3 /* DIN */, D2 /* DC */, D1 /* CE */, D0 /* RST */);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.setContrast(127);
  lcd.clear();
}

void print(String msg){
  Serial.print(msg);
  lcd.print(msg);
}
void println(String msg){
  Serial.println(msg);
  lcd.println(msg);
}
void clearLog(){
  lcd.clear();
}

bool wifiConnected = false;
void loop() {
  // WIFI
  if(!wifiConnected){
    print("1. Connecting to wifi");
  
    WiFi.begin(_wifi_ssid, _wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      print(".");
    }
    
    if(WiFi.status() != WL_CONNECTED){
      println("Connection failed");
      return;
    }
  }
  wifiConnected = true;

  // CALL XRP PRICE API
  print("2. Loading xrp price");

  char* apiUrl = "https://www.bitstamp.net/api/v2/ticker/xrpeur";

  // Info on how to get the thumbprint: http://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-secure-examples.html
  // The thumbprint is from the certificate, so it will change when the source changes it certificate
  char* thumbprint = "6D:53:E4:C7:8A:DF:46:F9:15:4F:A2:CD:10:67:C4:76:97:30:12:67";
  
  HTTPClient http;
  http.begin(apiUrl, thumbprint);
  int httpCode = http.GET();
  println("3. Got code " + String(httpCode));
  
  if(httpCode <= 0){
    println("No content to show");
    return;
  }

  String payloadJson = http.getString();
  
  http.end();
  
  // PARSE JSON
  StaticJsonBuffer<800> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(payloadJson);
  if (!root.success()) {
    println("Error pasing json");
    return;
  }

  const char* bidString = root["bid"];
  const char* askString = root["ask"];
  
  double bid = String(bidString).toFloat();
  double ask = String(askString).toFloat();
  double avgPrice = (bid+ask)/2;

  clearLog();
  println(String(avgPrice) + " EUR/XRP");

  long xrpWalletAmount = 47145; // Insert your wallet amount here! - random number used for example.
  double eurPriceInDkk = 7.44;
  long walletValueDkk = xrpWalletAmount * avgPrice * eurPriceInDkk;
  String walletValueDkkString = String(walletValueDkk / 1000) + "." + String(walletValueDkk % 1000);
  println(walletValueDkkString + " DKK");
  delay(100000);
}
