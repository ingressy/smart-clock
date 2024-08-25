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

int LED = 6;
int forwPin = 7;
//int backPin = 9;
int men = 0;
int start = 0;
char LED1 = HIGH;

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
  delay(2000);
}

void connectToWiFi() {
  if (WiFi.status() == WL_NO_MODULE) {
    digitalWrite(LED, HIGH);
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

  pinMode(LED, OUTPUT);
  pinMode(forwPin, INPUT);

  lcd.begin(16, 2);
  lcd.clear();

  dht.begin();

  lcd.print("m-cat v0.4 DEV");
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
  digitalWrite(LED, HIGH);

  auto timeZoneOffsetHours = 2;
  auto unixTime = timeClient.getEpochTime() + (timeZoneOffsetHours * 3600);
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
  lcd.print("Temperature:");
  lcd.print(Temperatur);
  lcd.print("\337C");
  delay(350);
}

void rtc() {
  timeClient.update();
  auto timeZoneOffsetHours = 2;
  auto unixTime = timeClient.getEpochTime() + (timeZoneOffsetHours * 3600);
  RTCTime timeToSet = RTCTime(unixTime);
  RTC.setTime(timeToSet);

  RTCTime currentTime;
  RTC.getTime(currentTime);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  if (currentTime.getHour() < 10) {
    lcd.setCursor(6, 0);
    lcd.print("0");
    lcd.print(currentTime.getHour());
  }
  else {
    lcd.print(currentTime.getHour());
  }
  lcd.print(":");
  if (currentTime.getMinutes() < 10) {
    lcd.setCursor(9, 0);
    lcd.print("0");
    lcd.print(currentTime.getMinutes());
  }
  else {
    lcd.print(currentTime.getMinutes());
  }
  lcd.print(":");
  if (currentTime.getSeconds() < 10) {
      lcd.setCursor(12, 0);
      lcd.print("0");
      lcd.print(currentTime.getSeconds());
  }
  else {
    lcd.print(currentTime.getSeconds());
  }
  

  lcd.setCursor(0, 1);
  lcd.print("Date: ");
  lcd.print(currentTime.getDayOfMonth());
  lcd.print(".");
  if (Month2int(currentTime.getMonth()) < 10) {
    lcd.setCursor(9, 1);
    lcd.print("0");
    lcd.print(Month2int(currentTime.getMonth()));
  }
  else {
    lcd.print(Month2int(currentTime.getMonth()));
  }
  lcd.print(".");
  lcd.print(currentTime.getYear());
  delay(350);
}

void menu() {
  if (men == 1) {
    if (start == 0) {
        digitalWrite(LED, LOW);
        start = 1;
    }
    rtc();
  } else if (men == 2) {
    temp();
  } else if (men == 3) {
    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print("Weather Bremen");
    lcd.setCursor(0, 1);
    lcd.print("Temperature:na\337C");
    delay(350);
    } else if (men == 4) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.write("Wind Speed:naBft");
    lcd.setCursor(0,1); 
    lcd.write("Gusts:     naBft");
    delay(350);
  } else if (men == 5) {
    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print("m-cat v0.4 DEV");
    lcd.setCursor(0,1); 
    lcd.print("by ingressy");
    delay(350);
  }
}
void mled() {
  if (dht.readTemperature() < 20) {
    digitalWrite(LED, HIGH);
    if (LED1 == HIGH) {
      LED1 = LOW;
      digitalWrite(LED, LOW);
    } else if (LED1 == LOW) {
      LED1 = HIGH;
      digitalWrite(LED, HIGH);
    }
  }
  if (dht.readTemperature() > 30) {
    digitalWrite(LED, HIGH);
    if (LED1 == HIGH) {
      LED1 = LOW;
      digitalWrite(LED, LOW);
    } else if (LED1 == LOW) {
      LED1 = HIGH;
      digitalWrite(LED, HIGH);
    }
  }
}
void loop() {
  menu();
  mled();

  if (digitalRead(forwPin)) {
    if (men == 0) {
      men = 1;
    } else if (men == 1) {
      men = 2;
    } else if (men == 2) {
      men = 3;
    } else if (men == 3) {
      men = 4;
    } else if (men == 4) {
      men = 5;
    } else if (men == 5) {
      men = 1;
    }
  } //else if (digitalRead(backPin)) {
      //  if (men == 0) {
      //men = 2;
    //} else if (men == 1) {
      //men = 0;
    //} else if (men == 2) {
      //men = 1;
    //}
}
