// Ebike intelligent battery capacity and range estimator
// Version 0.1 released: 13.1.2021
// Veikko Jaaskelainen

#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SoftwareSerial.h>

const int gpsTX = 3;
const int gpsRX = 4;

LiquidCrystal_I2C lcd(0x27,20,4);
SoftwareSerial gpsSerial(gpsTX, gpsRX);

const int sensorPin = A0;
const float digitalToCurrent = 1024 / 30; // rounds almost to 34


  
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.print("Welcome back!");
}

void loop() {
  // put your main code here, to run repeatedly:

}
