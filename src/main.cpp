#include <Arduino.h>
#include "calibration.h"

int GPIOmeas = 2;
int GPIOtrig = 3;
int p_PWM_Freq = 1000;
int p_PWM_Res = 8;

void serialRX()
{
  if (Serial.available())
  {
    String str = Serial.readStringUntil('\n');

    if (str.startsWith("CALCUR")) // starting check for "VAR"
    {
      Serial.println("Current meter calibration requested");
      calStart(str, GPIOmeas, GPIOtrig);
    }
  }
}

void setup() {
  pinMode(GPIOmeas, INPUT); 

  ledcAttachPin(GPIOtrig, 0);
  ledcSetup(0, p_PWM_Freq, p_PWM_Res);

  Serial.println("GPIO setup");
}

void loop() {
  serialRX();
}