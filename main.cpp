//lib
#include "DHT.h"
#include "RTC.h"
#include <LiquidCrystal.h>
#include <NTPClient.h>
#include <WiFiS3.h>
#include <WiFiUdp.h>

//port for temp
#define DHTPIN 13
#define DHTTYPE DHT11

//wifi name and pass
#define SECRET_SSID "ourprivatehomelan"
#define SECRET_PASS "ThisIs4OurPrivateHomeLAN"

int LED = 6;
int forwPin = 7;
int okPin = 8;
int backPin = 9;
int men = 0;
int start = 0;
char LED1 = HIGH;
int led_bli = 0;

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
  //pinModes 
  pinMode(LED, OUTPUT);
  pinMode(forwPin, INPUT);
  pinMode(okPin, INPUT);
  pinMode(backPin, INPUT);

  lcd.begin(16, 2);
  lcd.clear();

  dht.begin();

  //start screen
  lcd.print("m-cat v0.4 DEV");
  lcd.setCursor(0, 1);
  lcd.print("booting ...");

  //wifi and rtc
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

  //rtc time
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
  //repeat ntp update because the rtc clock goes strange
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

  //add nill
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
//code for the yellow led
void mled() {
  if (led_bli == 1) {
    digitalWrite(LED, HIGH);
    if (LED1 == HIGH) {
      LED1 = LOW;
      digitalWrite(LED, LOW);
    } else if (LED1 == LOW) {
      LED1 = HIGH;
      digitalWrite(LED, HIGH);
    } else {
      LED1 = HIGH;
    }
  } else if (led_bli == 0) {
    digitalWrite(LED, LOW);
  }

  if (dht.readTemperature() < 20) {
    led_bli = 1;
  }
  if (dht.readTemperature() > 30) {
    led_bli = 1;
  }
  //disable led
  if (dht.readTemperature() < 30) {
    led_bli = 0;
  }
  if (dht.readTemperature() > 20) {
    if (dht.readTemperature() > 30) {
      led_bli = 1;
    } else {
      led_bli = 0;
    }
  }
}
void loop() {
  menu();
  if (start == 1) {
    mled();
  }

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
  } else  if (digitalRead(okPin)) {
    if (men == 0) {
      men = 1; 
    }
  } else if (digitalRead(backPin)) {
    if (men == 0) {
      men = 1; 
    } else if (men == 1) {
      men = 5;
    } else if (men == 5) {
      men = 4;
    } else if (men == 4) {
      men = 3;
    } else if (men == 3) {
      men = 2;
    } else if (men == 2) {
      men = 1;
    }
  }
}
