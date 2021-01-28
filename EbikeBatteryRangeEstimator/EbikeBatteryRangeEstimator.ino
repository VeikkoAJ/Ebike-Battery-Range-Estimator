  // Ebike intelligent battery capacity and range estimator
// Version 0.5 released: 28.1.2021
// Veikko Jaaskelainen
extern "C"{
  #include "CellCapacity.h"
}

#include <Wire.h>
#include <SoftwareSerial.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

const int debug = 1; // set 1 to run voltage and lcd tests on startup

const int gpsTX = A1;
const int gpsRX = A2;
const int currentPin = A3;
const int voltagePin = A7;
const int lightSwitchPin = 12;
const int highBeamSwitchPin = 3;
const int lightDriverPin = 11;

hd44780_I2Cexp lcd(0x27);
SoftwareSerial gpsSerial(gpsTX, gpsRX);


const float currentShift = 0.25;// calibrate current sensor 
const float digitalToCurrent = 30.0 / (512.0);
const float voltageLevelShift = 59.2 / 1023.0;


// size of the battrey grid
const int bRows = 13;
const int bColumns = 3;
const float cellCapacity = 2.55;
const float rowAh = (float) bColumns * cellCapacity;

// headlight levels
const int lowLight = 10;
const int highLight = 50;
const int fullLight = 255;

int lightState = 0;
int beamState = 0;

int standBy = 0; // current  < 0.2
float vDOD = 0.0; //depth of discharge, from voltage
float DOD = 0.0; //depth of discharge
float SOC = 0.0; // state of charge (percentage)
float current = 0.0;
float voltage = 0.0;

unsigned long currentMeasureMillis = 0; // used for current integration
unsigned long lcdRefreshMillis = 0; // used for lcd refreshing
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


// Sets the vDOD according to the predefined discharge graph.  
void setVDOD(float v) {
  float x = v * 1000 /  bRows;
  float Ah = getCapacity(x, (int) (cellCapacity * 1000));
  // function for charge(v) from 18650 discharge graph. Check GitHub for more information.
  vDOD = (float) Ah / 1000.0 * (float) bColumns;
}

void setDOD() {
  if (current < 0.2) {
    setVDOD(voltage);
    if (standBy) {
      DOD = (DOD * 7 + vDOD) / 8;
    } else {
      DOD = vDOD;
    }
    standBy = 1;
  };
  if (current > 0.5) {
     standBy = 0;
  }
}
 
void refreshLCD() {
  lcd.clear();
    lcd.print(current);
    lcd.print("A ");
    lcd.print(voltage);
    lcd.print('V');
    lcd.setCursor(0,1);
    lcd.print(DOD);
    lcd.print("Ah ");
    lcd.print(SOC * 100);
    lcd.print("%");
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
  if (debug) {
    lcd.print("DEBUG = 1");
    delay(500);
    lcd.clear();
    delay(500);
    for (int i = 110; i > 0; i--) {
      voltage = (float) i / 2.0;
      current = 0.1;
      setVDOD(voltage);
      DOD = vDOD;
      SOC = 1.0 - vDOD / rowAh;
      refreshLCD();
      delay(500);
    }
    lcd.clear();
    lcd.print("DEBUG OVER");
  }
  measureVA();
  setDOD();
  bootAnimation();
}

void loop() {
  setLights();
  measureVA();
  setDOD();
  //current integration
  delay(250);
  unsigned long measureTime = (millis() - currentMeasureMillis) / 1000;
  DOD = DOD - current * measureTime / 3600;
  SOC = DOD / rowAh;
  currentMeasureMillis = millis();

  if (millis() - lcdRefreshMillis > 1500) {
    refreshLCD();
    lcdRefreshMillis = millis();
  }
}
