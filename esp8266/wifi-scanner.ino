#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

struct WifiNetwork {
  String ssid;
  int8_t rssi;
};

Adafruit_SSD1306 oledDisplay(0); // GPIO0
WifiNetwork networks[20];
unsigned int ScanDelay = 2000; // in ms



void setupSerial() {
  Serial.begin(115200);
  Serial.println();
}

void setupLed() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void setupOledDisplay() {
  oledDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oledDisplay.clearDisplay();
  oledDisplay.setTextSize(1);
  oledDisplay.setTextColor(WHITE);
  oledDisplay.setCursor(0, 0);
  oledDisplay.display();
}

void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
}



void setup() {
  setupSerial();
  setupLed();
  setupOledDisplay();
  setupWifi();  
  delay(1000);
}

void ledBlink(int8_t times) {
  for (int8_t i = 0; i < times; i++) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(25);
    digitalWrite(LED_BUILTIN, HIGH);
    if (times > 1) {
      delay(100);
    }
  }
}

void loop() {
  ledBlink(1);
  Serial.print("WiFi Networks scan start...");
  int n = WiFi.scanNetworks();
  ledBlink(2);
  Serial.println(" done");

  for (int i = 0; i < 20; i++) {
    if (i < n) {
      networks[i].ssid = WiFi.SSID(i);
      networks[i].rssi = WiFi.RSSI(i);
    } else {
      networks[i].ssid = "---";
      networks[i].rssi = -124;
    }
  }

  // from https://forum.arduino.cc/index.php?topic=643560.0
  auto cmpfunc = [](WifiNetwork a, WifiNetwork b) { return a.rssi > b.rssi; }; 
  std::sort(std::begin(networks), std::end(networks), cmpfunc);

//  for (WifiNetwork item: networks) {
//    Serial.print(item.rssi);
//    Serial.print(" "+ item.ssid);
//    Serial.println();
//  }

  oledDisplay.clearDisplay();
  oledDisplay.setCursor(0, 0);

  for (int8_t i = 0; i < 6; i++) { 
    Serial.printf("%d %s\n", networks[i].rssi, networks[i].ssid.substring(0, 7).c_str());
    oledDisplay.printf("%d %s\n", abs(networks[i].rssi), networks[i].ssid.substring(0, 7).c_str());   
  }
  
  oledDisplay.display();
  Serial.println();
  
  delay(ScanDelay);
}
