//biblo
#include "DHT.h"

//port für temp
#define DHTPIN 13
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();

}

void loop() {
  delay(2000);
  float Luftfeuchtigkeit = dht.readHumidity();
  float Temperatur = dht.readTemperature();

  Serial.print("Luftfeuchtigkeit: ");
  Serial.print(Luftfeuchtigkeit);
  Serial.println(" %");

  Serial.print("Temperatur: ");
  Serial.print(Temperatur);
  Serial.println(" Grad Celsisus");
}
