//biblo
#include "DHT.h"
#include "RTC.h"
#include <LiquidCrystal.h>
#include <NTPClient.h>
#include <WiFiS3.h>
#include <WiFiUdp.h>

//port für temp
#define DHTPIN 13
#define DHTTYPE DHT11
#define SECRET_SSID "xxx" //network name
#define SECRET_PASS "xxx" //network password

int ErrorLED = 7;

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

int wifiStatus = WL_IDLE_STATUS;
WiFiUDP Udp;
NTPClient timeClient(Udp);

//Parameters: (rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

DHT dht(DHTPIN, DHTTYPE);

void printWiFiStatus() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(WiFi.SSID());

  lcd.setCursor(0, 1);
  IPAddress ip = WiFi.localIP();
  lcd.print(ip);
  delay(3000);
}

void connectToWiFi() {
  if (WiFi.status() == WL_NO_MODULE) {
    digitalWrite(ErrorLED, HIGH);
    while (true);
  }

  while (wifiStatus != WL_CONNECTED) {
    wifiStatus = WiFi.begin(ssid, pass);
    delay(10000);
    printWiFiStatus();
  }
}

void setup() {
  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.clear();

  dht.begin();

  lcd.print("mcat v0.2");
  lcd.setCursor(0, 1);
  lcd.print("booting ...");

  connectToWiFi();
  RTC.begin();
  timeClient.begin();
  timeClient.update();

  delay(1000);
  lcd.clear();

  auto timeZoneOffsetHours = 2;
  auto unixTime = timeClient.getEpochTime() + (timeZoneOffsetHours * 3600);
  Serial.print("Unix time = ");
  Serial.println(unixTime);
  RTCTime timeToSet = RTCTime(unixTime);
  RTC.setTime(timeToSet);
}

void temp() {
  int Luftfeuchtigkeit = dht.readHumidity();
  int Temperatur = dht.readTemperature();

  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(Luftfeuchtigkeit);
  lcd.print("% Luftf.");

  lcd.setCursor(0, 1);
  lcd.print(Temperatur);
  lcd.print("C Temp.");
}

void loop() {
  RTCTime currentTime;
  RTC.getTime(currentTime);
  lcd.setCursor(0, 0);
  lcd.print(currentTime.getHour());
  lcd.print(":");
  lcd.print(currentTime.getMinutes());
  lcd.print(":");
  lcd.print(currentTime.getSeconds());

  lcd.setCursor(0, 1);
  lcd.print(currentTime.getDayOfMonth());
  lcd.print("/");
  lcd.print(Month2int(currentTime.getMonth()));
  lcd.print("/");
  lcd.print(currentTime.getYear());
}
