#include <LiquidCrystal.h>
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

// Voltage #1 Sensor (Analog 0)
const int analogInPin = A0;

// Temperature/Humidity Sensor (blue square)
#define DHTPIN 7    // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

// Define the digital pins for each sensor
#define ONE_WIRE_BUS_SENSOR_1 6
#define ONE_WIRE_BUS_SENSOR_2 8
OneWire oneWireSensor1(ONE_WIRE_BUS_SENSOR_1);
OneWire oneWireSensor2(ONE_WIRE_BUS_SENSOR_2);
DallasTemperature sensors1(&oneWireSensor1);
DallasTemperature sensors2(&oneWireSensor2);

SoftwareSerial mySerial(0, 1); // RX, TX

// Resistor values for the voltage divider
// Ensure these EXACTLY match the resistors you used
float R1 = 30000.0;
float R2 = 7500.0; 

// Maximum ADC reading (usually 1023 for 10-bit ADC)
const int ADC_MAX_VALUE = 1023;

const float ADC_REF_VOLTAGE = 5; // Volts

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  
  // initialize the serial communications:
  Serial.begin(9600);
  mySerial.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();
  
  pinMode(analogInPin, INPUT);
  dht.begin();
  sensors1.begin();
  sensors2.begin();
}

void loop() {
  // Voltage from A0
  int adcValue = analogRead(analogInPin);
  float vOut = (adcValue * ADC_REF_VOLTAGE) / ADC_MAX_VALUE;
  float vIn = vOut * (R1 + R2) / R2;

  // Temperature & Humidity
  float h = dht.readHumidity();
  float t1 = dht.readTemperature(true); //Fahrenheit

   // Temperature Sensor #1 (Black Wire)
  sensors1.requestTemperatures();
  float t2 = sensors1.getTempFByIndex(0);

  sensors2.requestTemperatures();
  float t3 = sensors2.getTempFByIndex(0);

  // Print all the data in serial
  Serial.print(vIn);
  Serial.print(",");
  Serial.print(h);
  Serial.print(",");
  Serial.print(t1);
  Serial.print(",");
  Serial.print(t2);
  Serial.print(",");
  Serial.print(t3);
  Serial.print(",\n");

   // Print all the data in serial
  mySerial.print(vIn);
  mySerial.print(",");
  mySerial.print(h);
  mySerial.print(",");
  mySerial.print(t1);
  mySerial.print(",");
  mySerial.print(t2);
  mySerial.print(",");
  mySerial.print(t3);
  mySerial.print(",\n");

  // Print in the LCD
  lcd.setCursor(0, 0);
  lcd.print("T1:");
  lcd.setCursor(3, 0);
  lcd.print(t1,1);
  lcd.setCursor(7, 0);
  lcd.print("F");

  lcd.setCursor(9, 0);
  lcd.print("H:");
  lcd.setCursor(11, 0);
  lcd.print(h);
  lcd.setCursor(15, 0);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("T2:");
  lcd.setCursor(3, 1);
  lcd.print(t2,1);
  lcd.setCursor(7, 1);
  lcd.print("F");

  lcd.setCursor(9, 1);
  lcd.print("V1:");
  lcd.setCursor(12, 1);
  lcd.print(vIn,1);
  lcd.setCursor(15, 1);
  lcd.print("V");


  delay(2000); // Wait 2 seconds before the next reading
}