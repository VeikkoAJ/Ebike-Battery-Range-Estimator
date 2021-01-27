  // Ebike intelligent battery capacity and range estimator
// Version 0.1 released: 13.1.2021
// Veikko Jaaskelainen
#include <Wire.h>
#include <SoftwareSerial.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

const int gpsTX = A1;
const int gpsRX = A2;
const int currentPin = A3;
const int voltagePin = A7;
const int lightSwitchPin = 12;
const int highBeamSwitchPin = 3;
const int lightDriverPin = 11;

// 
const float currentShift = 0.25;

hd44780_I2Cexp lcd(0x27);
SoftwareSerial gpsSerial(gpsTX, gpsRX);


const float digitalToCurrent = 30.0 / (512.0);
const float voltageLevelShift = 59.2 / 1023.0;

const int lowLight = 10;
const int highLight = 50;
const int fullLight = 255;

int lightState = 0;
int beamState = 0;
float current = 0.0;
float voltage = 0.0;

// nice animation to check lcd and front light works
void bootAnimation() {
  int animationStep = 80;
  for (int i = 255; i >= 0; i--) {
    delay(animationStep / 17);
    analogWrite(lightDriverPin, 255 - i);
    if (i % 17 == 0) {
      lcd.setCursor(i / 17, 0);
      lcd.write((char) 255);
      lcd.setCursor(i / 17, 1);
      lcd.write((char) 255);
    }
  }
  delay(300);
  for (int i = 0; i <= 255; i++) {
    delay(animationStep / 17);
    analogWrite(lightDriverPin, (255 - i) * (255 - lowLight) / 255 + lowLight );
    if (i % 17 == 0) {
      lcd.setCursor(i / 17, 0);
      lcd.write('=');
      lcd.setCursor(i / 17, 1);
      lcd.write('=');
    }
  }
  delay(200);
  for (int i = 0; i < 256; i++) {
    delay(animationStep / 17);
    if (i % 17 == 0) {
      lcd.setCursor(i / 17, 0);
      lcd.write(' ');
      lcd.setCursor(i / 17, 1);
      lcd.write(' ');
    }
  }
}

// sets the PWM driven light to correct state
void setLights() {
 // light state detection
  lightState = digitalRead(lightSwitchPin);
  beamState = digitalRead(highBeamSwitchPin);
  // set front light to current level
  if (lightState == 1 && beamState == 1) {
    analogWrite(lightDriverPin, fullLight);
  } else if (lightState == 1) {
    analogWrite(lightDriverPin, highLight);
  } else {
    analogWrite(lightDriverPin, lowLight);
  }

}

void measureVA() {
  float tempCurrent = 0.0;
  float tempVoltage = 0.0;
  const int iterations = 50;
  for (int i = 0; i < iterations; i++) {
    tempCurrent = tempCurrent + (analogRead(currentPin) - 512.0) * digitalToCurrent + currentShift;
    tempVoltage = tempVoltage + analogRead(voltagePin) * voltageLevelShift;
    delay(1);
  }
  current = tempCurrent / (float) iterations;
  voltage = tempVoltage / (float) iterations;
}
 
void setup() {
  pinMode(currentPin, INPUT);
  pinMode(voltagePin, INPUT);
  pinMode(lightSwitchPin, INPUT);
  pinMode(highBeamSwitchPin, INPUT);
  pinMode(lightDriverPin, OUTPUT);
    
  int status;
  status = lcd.begin(16, 2);
  if(status) {
    hd44780::fatalError(status); // does not return
  }
  bootAnimation();
}

void loop() {
  lcd.clear();
  setLights();
  measureVA();
  lcd.print(current);
  lcd.print('A');
  lcd.setCursor(0,1);
  lcd.print(voltage);
  lcd.print('V');
  delay(500);
}
