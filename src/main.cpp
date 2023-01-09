#include <Arduino.h>
#include "calibration.h"

int GPIOmeas = 34;
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
      cal01.calStart(str, GPIOmeas, GPIOtrig);
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
  pinMode(GPIOmeas, INPUT); 
  ledcAttachPin(GPIOtrig, 0);
  ledcSetup(0, p_PWM_Freq, p_PWM_Res);
  Serial.println("GPIO setup");
}

void loop() {
  serialRX();
  cal01.measure(GPIOmeas); //the ticker for the current filter
  delay(10);
}

