#include <Arduino.h>
#include "calibration.h"

int GPIOcurrentMeas = 34;
int GPIOvoltageMeas = 35;
int GPIOtrig = 33;
int p_PWM_Freq = 1000;
int p_PWM_Res = 8;

cal cal01; 

void serialRX()
{
  if (Serial.available())
  {
    String str = Serial.readStringUntil('\n');

    if (str.startsWith("CALCUR")) // starting check for "VAR"
    {
      Serial.println("Current meter calibration requested");
      cal01.calStart(str, GPIOcurrentMeas, GPIOtrig);
    }
    if (str.startsWith("DUMP")) // starting check for "VAR"
    {
      Serial.println("log dump");
      cal01.logDump();
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(GPIOcurrentMeas, INPUT); 
  ledcAttachPin(GPIOtrig, 0);
  ledcSetup(0, p_PWM_Freq, p_PWM_Res);
  pinMode(GPIOvoltageMeas, INPUT);

  cal01.setup(GPIOcurrentMeas, GPIOvoltageMeas);
  Serial.println("GPIO setup");
}

void loop() {
  serialRX();
  cal01.task20ms(); //the ticker for the current filter
  delay(20);
}

