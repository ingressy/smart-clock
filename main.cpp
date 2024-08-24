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

const int ErrorLED = 6;
int x = 7;
int val=0, men=0;

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

  pinMode(ErrorLED, OUTPUT);

  lcd.begin(16, 2);
  lcd.clear();

  dht.begin();

  lcd.print("m-cat v0.3");
  lcd.setCursor(0, 1);
  lcd.print("booting ...");

  connectToWiFi();
  RTC.begin();
  timeClient.begin();
  timeClient.update();

  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("     -meow");
  lcd.setCursor(1, 1);
  lcd.print("i am hungry ^^");

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

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Humidity:   ");
  lcd.print(Luftfeuchtigkeit);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("Temperatur: ");
  lcd.print(Temperatur);
  lcd.print("\337C");
}

void rtc() {
  auto timeZoneOffsetHours = 2;
  auto unixTime = timeClient.getEpochTime() + (timeZoneOffsetHours * 3600);
  RTCTime timeToSet = RTCTime(unixTime);
  RTC.setTime(timeToSet);
  RTCTime currentTime;
  RTC.getTime(currentTime);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Zeit:  ");
  lcd.print(currentTime.getHour());
  lcd.print(":");
  if (currentTime.getMinutes() < 10) {
    lcd.setCursor(9, 0);
    lcd.print("0");
    lcd.print(currentTime.getMinutes());
    }
  else {
    lcd.print(currentTime.getMinutes());

  }

  lcd.setCursor(0, 1);
  lcd.print("Datum: ");
  lcd.print(currentTime.getDayOfMonth());
  lcd.print(".");
  lcd.print(Month2int(currentTime.getMonth()));
  lcd.print(".");
  lcd.print(currentTime.getYear());
}
void loop() {
  val=digitalRead(x);
  if(val==1){
    men=men+1;
  }
  
  if (men==1){
    rtc();
  }
  else if (men==2){
    temp();
  }
  else if (men==3){
    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print(" Mode 3 ");
  }

  else if (men==4){
    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print("m-cat v0.3");
    lcd.setCursor(0,1); 
    lcd.print("by ingressy");
    men=1;
  }

  //wait 300 ms for the next reading
  delay(30); 
}
