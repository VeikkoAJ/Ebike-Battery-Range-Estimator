  // Ebike intelligent battery capacity and range estimator
// Version 0.5 released: 28.1.2021
// Veikko Jaaskelainen
extern "C"{
  #include "CellCapacity.h"
  #include "CustomChar.h"
}

#include <Wire.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

const int debug = 1; // Run voltage and lcd tests on startup
const int debugGps = 0; // enables serial communication to pc from gps
const int debugRange = 1; // debugging range estimation

const int gpsTX = A2;
const int gpsRX = A1;
const int currentPin = A3;
const int voltagePin = A7;
const int lightSwitchPin = 12;
const int highBeamSwitchPin = 3;
const int lightDriverPin = 11;

SoftwareSerial gpsSerial(gpsTX, gpsRX);
TinyGPS gps;
hd44780_I2Cexp lcd(0x27);


const float currentShift = 0.25;// calibrate current sensor 
const float digitalToCurrent = 30.0 / (512.0);
const float voltageLevelShift = 59.2 / 1023.0;


// size of the battrey grid
const int bRows = 13;
const int bColumns = 3;
const float cellCapacity = 2.6;
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
int range = 0;

long rangeTimeSum = 0;
float rangeCurrentAverage = 0.0;
float rangeVelocityAverage = 0.0;
int rangeSumCount = 0;

float velocityKnots = 0.0;
bool newGpsData = false; 

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
  for (int i = 0; i < 256; i++) {
    delay(animationStep / 17);
    float lighValue = (255 - i ) > lowLight ? (255 - i) : lowLight;
    analogWrite(lightDriverPin, lighValue);
    if (i % 17 == 0) {
      lcd.setCursor(i / 17, 0);
      lcd.write('=');
      lcd.setCursor(i / 17, 1);
      lcd.write('=');
    }
  }
  delay(100);
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

// sets the lcd custom characters
void setCustomChars() {
  const int numbers[8] = {0,2,4,5,6,7,8,9};
  byte customChar[8];
  for (int i = 0; i < 8; i++) {
    getCustomCharPattern(numbers[i], customChar);
    lcd.createChar(i, customChar);
  }
} 

void printReMappedString(char inputText[], int column) {
  if (column > 15 || column < 0) {
    return;
  }
  unsigned char text[2];
  remapText(inputText, text);
  lcd.setCursor(column, 1);
  lcd.write(byte(text[0]));
  if (strlen(inputText) < 2) {
    return;
  }
  lcd.setCursor(column, 0);
  lcd.write(byte(text[1]));
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
  tempCurrent = tempCurrent / (float) iterations;
  current = tempCurrent > 0 ? tempCurrent : 0.0; // prevents negative current (negative current is useless, but can occur with motor braking)
  voltage = tempVoltage / (float) iterations;
}

// Sets the vDOD according to the predefined discharge graph.  

void setVDOD(float v) {
  int mAh = 0;
  int x = v * 1000.0 /  bRows;
  mAh = getCapacity(x, (int) (cellCapacity * 1000));
  vDOD = (float) mAh * bColumns / 1000.0;
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
 
void setRangeEstimation(unsigned long timeInterval) {
  if (rangeSumCount < 20) {
    if (debugRange) {
      current = 8.15;
      velocityKnots = 13.95;
      Serial.println(rangeSumCount);
    }
    rangeTimeSum += timeInterval;
    rangeVelocityAverage += velocityKnots;
    rangeCurrentAverage += current;
    rangeSumCount++; 
  } else {
    rangeSumCount = 0;
    float usedAh = (rangeCurrentAverage / 20.0 * ((float) rangeTimeSum / 3600.0));
    float hoursLeft = (rowAh - DOD) / usedAh * ((float) rangeTimeSum / 3600.0);
    if (debugRange) {
      Serial.print("current=");
      Serial.print(rangeCurrentAverage / 20.0);
      Serial.print(", used amp hours=");
      Serial.print(usedAh);
      Serial.print(", batt amp hours=");
      Serial.print((rowAh - DOD));
      Serial.print(", velocity=");
      Serial.print(rangeVelocityAverage * 1.852 / 20.0);
      Serial.print(", time=");
      Serial.print(rangeTimeSum);
      Serial.print(", hours left=");
      Serial.print(hoursLeft);
    }
    float newRange = hoursLeft * rangeVelocityAverage * 1.852 / 20.0;
    if (newRange < 0.0) {
      range = 0;
    } else if(newRange > 99.0) {
      range = 99;
    } else {
      range = newRange;
    }
    if (debugRange) {
      Serial.print(", range=");
      Serial.println(newRange);
    }
    rangeSumCount = 0;
    rangeCurrentAverage = 0.0;
    rangeVelocityAverage = 0.0;
    rangeTimeSum = 0;
  }
}

void refreshLCD() {
  lcd.clear();
  int printColumn = 0;
  // battery charge bar
  for (int i = 0; i <= 10 - (int) (SOC * 10.0 + 1); i++) {
    lcd.setCursor(i, 0);
    lcd.write(" ");
    lcd.setCursor(i, 1);
    lcd.write(" ");
    printColumn = i;
  }
  unsigned char middleSymbol;
  int decimalPercentage = int (SOC * 100.0) - int (SOC * 10) * 10;
  if (decimalPercentage < 3) {
    middleSymbol = ' ';
  }
  if (decimalPercentage <= 6) {
    middleSymbol = byte(0);
  }
  if (decimalPercentage > 6) {
    middleSymbol = (char) 255;
  }
  lcd.setCursor(printColumn + 1, 0);
  lcd.write(middleSymbol);
  lcd.setCursor(printColumn + 1, 1);
  lcd.write(middleSymbol);
  for (int i = printColumn + 2; i <= int (SOC * 10.0) + printColumn; i++) {
    lcd.setCursor(i, 0);
    lcd.write((char) 255);
    lcd.setCursor(i, 1);
    lcd.write((char) 255);
  }
  // divider
  lcd.setCursor(10, 0);
  lcd.write("|");
  lcd.setCursor(10, 1);
  lcd.write("|");
  // velocity
  char velocityText[3];
  if (velocityKnots > 0) {
    printReMappedString(itoa(velocityKnots * 1.852, velocityText, 10), 13);
  } else {
    printReMappedString(itoa(00, velocityText, 10), 13);
  }
  
  // range
  char rangeText[3];
  if (velocityKnots * 1.852 > 5) {
    printReMappedString(itoa(range, rangeText, 10), 15);
  } else {
    printReMappedString(itoa(99, rangeText, 10), 15);
  }
}

void setup() {
  pinMode(currentPin, INPUT);
  pinMode(voltagePin, INPUT);
  pinMode(lightSwitchPin, INPUT);
  pinMode(highBeamSwitchPin, INPUT);
  pinMode(lightDriverPin, OUTPUT);

  Serial.begin(115200);
  gpsSerial.begin(9600);
  Serial.println("setting up"); 
  // lcd setup
  int status;
  status = lcd.begin(16, 2);
  if(status) {
    hd44780::fatalError(status); // does not return
  }
  setCustomChars();
  //debug mode to display characters on lcd and test the voltage to capacity function
  if (debug) {
    lcd.print("DEBUG = 1");
    delay(200);
    lcd.clear();
    delay(200);
    for (int i = 0; i < 15; i++) {
      char text[3];
      printReMappedString(itoa(i, text, 10), i);
    }
    delay(1500);
    lcd.clear();
    delay(200);
    for (int i = 110; i > 76; i--) {
      voltage = (float) i / 2.0;
      current = 0.1;
      setVDOD(voltage);
      DOD = vDOD;
      SOC = 1.0 - vDOD / rowAh;
      refreshLCD();
      delay(200);
    }
    lcd.clear();
    lcd.print("DEBUG OVER");
    Serial.println("debug over");
  }
  measureVA();
  setDOD();
  bootAnimation();  
}

void loop() {
  setLights();
  newGpsData = false;
  unsigned long chars;
  unsigned short sentences, failed;
  
  for (unsigned long start = millis(); millis() - start < 1000;) {
    while (gpsSerial.available() &&  millis() - start < 1000) {
      char c = gpsSerial.read();
      if (debugGps) {
        Serial.write(c);
      }
      if (gps.encode(c)) {
        newGpsData = true;
      }
    }
  }
  if (newGpsData) {
    velocityKnots = gps.speed();;
  }
  if (debugGps) {
    Serial.print("\n\nsatellites=");
    Serial.println(gps.satellites());
    Serial.print("velocityKnots=");
    Serial.println(velocityKnots);
    gps.stats(&chars, &sentences, &failed);
    Serial.print("CHARS=");
    Serial.print(chars);
    Serial.print(" SENTENCES=");
    Serial.print(sentences);
    Serial.print(" CSUM ERR=");
    Serial.println(failed);
    Serial.println("\n\n");
  }
  
  setLights();
  measureVA();
  setDOD();
  
  //current integration
  if (millis() - currentMeasureMillis > 1000) {
    unsigned long measureTime = (millis() - currentMeasureMillis) / 1000;
  DOD = DOD - current * measureTime / 3600;
  SOC = 1.0 - (DOD / rowAh);
  setRangeEstimation(measureTime);
  currentMeasureMillis = millis();
  }
  

  //only refresh screen 0.66 Hz
  if (millis() - lcdRefreshMillis > 1500) {
    refreshLCD();
    lcdRefreshMillis = millis();
  }
}
