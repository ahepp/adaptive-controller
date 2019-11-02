#include <HID.h>
#include <Keyboard.h>
#include <Mouse.h>
#include <Wire.h>

#include "Adafruit_MPRLS.h"

// General settings
#define BAUD_RATE 115200
#define MS_WAIT 10

// Sensor settings
#define DEBUG_SENSORS 1
#define AREF    3.3
#define ARES 1023.0
#define PSI_FACTOR 0.01450377377
#define RESET_PIN  -1
#define EOC_PIN    5

// Detection settings
#define POT_CENTER    1.65
#define POT_THRESHOLD 0.15
#define ATM_PRESSURE  14.4
#define SIP_THRESHOLD 11.0
#define PUF_THRESHOLD 15.5

// Mouse settings
#define DEBUG_CLICKS 0
#define DEBUG_PTR 0
#define PTR_SPEED 5
#define ENABLE_MOUSE 5


Adafruit_MPRLS mpr = Adafruit_MPRLS(RESET_PIN, EOC_PIN);

float scale(int analog_read_val);

void risingPufCallback();
void fallingPufCallback();

void risingSipCallback();
void fallingSipCallback();

float lastPressure;

void setup() {
  Serial.begin(BAUD_RATE);
  if(ENABLE_MOUSE) {
    Mouse.begin();
  }

  if (! mpr.begin()) {
    Serial.println("Failed to connect to pressure sensor");
  }

  lastPressure = mpr.readPressure() * PSI_FACTOR;
}

void loop() {
  float vA2 = scale(analogRead(A2)); // enable
  if(vA2 > 3.0) {
    // Read sensors
    // This is very readable, but *extremely* inefficient
    float vA0 = scale(analogRead(A0));
    float vA1 = scale(analogRead(A1));
    float pressure = mpr.readPressure() * PSI_FACTOR;
    if(DEBUG_SENSORS) {
      Serial.print(vA0);
      Serial.print("\t/\t");
      Serial.print(vA1);
      Serial.print("\t/\t");
      Serial.println(pressure);
    }

    if(pressure > PUF_THRESHOLD) {
      if(!(lastPressure > PUF_THRESHOLD)) {
        risingPufCallback();
      }
    }
    else if (lastPressure > PUF_THRESHOLD) {
      fallingPufCallback();
    }
    if(pressure < SIP_THRESHOLD) {
      if(!(lastPressure < SIP_THRESHOLD)) {
        risingSipCallback();
      }
    }
    else if (lastPressure < SIP_THRESHOLD) {
      fallingSipCallback();
    }
    lastPressure = pressure;

    // Move ptr
    char x = 0;
    char y = 0;
    if(vA0 > POT_CENTER + POT_THRESHOLD) {
      x = vA0;
    }
    else if(vA0 < POT_CENTER - POT_THRESHOLD) {
      x = vA0 - AREF;
    }
    
    // Detect axis 1 position
    if(vA1 > POT_CENTER + POT_THRESHOLD) {
      y = vA1;
    }
    else if(vA1 < POT_CENTER - POT_THRESHOLD) {
      y = vA1 - AREF;
    }
    Mouse.move(x * PTR_SPEED, -y * PTR_SPEED, 0);
    
  }
  delay(MS_WAIT);
}

float scale(int analog_read_val) {
  return analog_read_val * (AREF / ARES);
}

void risingPufCallback() {
  if(DEBUG_CLICKS) {Serial.println("right press");}
  Mouse.press(MOUSE_RIGHT);
}
void fallingPufCallback() {
  if(DEBUG_CLICKS) {Serial.println("right release");}
  Mouse.release(MOUSE_RIGHT);
}
void risingSipCallback() {
  if(DEBUG_CLICKS) {Serial.println("left press");}
  Mouse.press(MOUSE_LEFT);
}
void fallingSipCallback() {
  if(DEBUG_CLICKS) {Serial.println("left release");}
  Mouse.release(MOUSE_LEFT);
}
