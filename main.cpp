//biblo
#include "DHT.h"
#include <LiquidCrystal.h>

//port für temp
#define DHTPIN 13
#define DHTTYPE DHT11
//Parameters: (rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();

  dht.begin();

  lcd.print("mcat v0.1");
  lcd.setCursor(0, 1);
  lcd.print("booting ...");
  delay(1000);
  lcd.clear();
}

void loop() {
  int Luftfeuchtigkeit = dht.readHumidity();
  double Temperatur = dht.readTemperature();

  delay(1000);
  lcd.setCursor(0, 0);
  lcd.print(Luftfeuchtigkeit);
  lcd.setCursor(2, 0);
  lcd.print("% Luftf.");

  lcd.setCursor(0, 1);
  lcd.print(Temperatur);
  lcd.setCursor(2, 1);
  lcd.print("C Temp.");

}
