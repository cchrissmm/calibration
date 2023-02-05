#include <arduino.h>
#include "calibration.h"
#include "logger.h"
#include <EEPROM.h>

logger log01("measure","raw,filt");

// Expecting this as input
//"CALCUR,00070,04670" meaning calibrate the current sensor expecting 70mA with no load and 4670mA with load"
//"CALVOL,12100" meaning calibrate the voltage sensor expecting 12100mV

// constructor
cal::cal() {
}

void cal::setup(int cPin, int vPin, int tPin) {
    this -> currentPin = cPin;
    this -> voltagePin = vPin;
    this-> triggerPin = tPin;
    Serial.println("calibration setup with pins current: " + String(cPin) + " voltage: " + String(vPin) + " trigger: " + String(tPin));
    cal::readCal();
}

void cal::task20ms() {
    cal::getCurrentF();
    cal::getVoltageF();
}

void cal::calStart(String str)
{
    if (str.startsWith("CALCUR,")) // starting check for "CALCUR"
    {
        Serial.println("Current meter calibration request received: " + str);
        String y1Str = str.substring(7, 12); // the index positions of the code
        Serial.print("point y1: ");
        float y1 = y1Str.toFloat();
        Serial.println(y1);
        if (!y1)
        {
            Serial.println("point y1 contains an error: ");
            Serial.println(y1Str);
            return;
        }
        Serial.print("measuring x1 (output off state): ");
        analogWrite(triggerPin, 0);
        float x1 = cal::measCal(currentPin);
        Serial.println(x1);

        String y2Str = str.substring(13, 18); // the index positions of the code
        Serial.print("point y2: ");
        float y2 = y2Str.toFloat();
        Serial.println(y2);
        if (!y2)
        {
            Serial.println("point y2 contains an error: ");
            Serial.println(y2Str);
            return;
        }
      
        Serial.print("measuring x2 (output on state): ");
        analogWrite(triggerPin, 255);
        float x2 = cal::measCal(currentPin);
        Serial.println(x2);
        analogWrite(triggerPin, 0);

        cal::fitCurrentEqn(x1, y1, x2, y2);

        cal::writeCurrentCal();
    }

    if (str.startsWith("CALVOL,")) // starting check for "CALVOL"
    {
        Serial.println("Voltage meter calibration request received: " + str);
        String y2Str = str.substring(7, 12); // the index positions of the code
        Serial.print("point y2: ");
        float y2 = y2Str.toFloat();
        Serial.println(y2);
        if (!y2)
        {
            Serial.println("point y2 contains an error: ");
            Serial.println(y2Str);
            return;
        }
        Serial.print("measuring x2: ");
        float x2 = cal::measCal(voltagePin);
        Serial.println(x2);
        cal::fitVoltageEqn(0, 0, x2, y2);
        cal::writeVoltageCal();
    }
}

//***read the input port a number of times and average it
float cal::measCal(int measPin)
{
    int sumMeasuredVal = 0;
    delay(3000); //let the signal settle first
    for (int i = 0; i < 10; i++)
    {
        int measuredVal = analogRead(measPin);
        sumMeasuredVal += measuredVal;
        //Serial.println(measuredVal);
        delay(10);
    }
    float settledVal = sumMeasuredVal / 10;
    Serial.print("settled val: ");
    Serial.println(settledVal);
    return settledVal;
}

//***get the linearfunc
void cal::fitCurrentEqn(float x1, float y1, float x2, float y2)
{
    // calculate the slope of the line
    currentSlope = (y2 - y1) / (x2 - x1);
    Serial.print("currentSlope is ");
    Serial.println(currentSlope);

    // calculate the y-intercept of the line
    currentYint = y1 - (currentSlope * x1);
    Serial.print("y int is ");
    Serial.println(currentYint);
}

//***get the linearfunc
void cal::fitVoltageEqn(float x1, float y1, float x2, float y2)
{
    // calculate the slope of the line
    voltageSlope = (y2 - y1) / (x2 - x1);
    Serial.print("voltageSlope is ");
    Serial.println(voltageSlope);

    // calculate the y-intercept of the line
    voltageYint = y1 - (voltageSlope * x1);
    Serial.print("y int is ");
    Serial.println(voltageYint);
}

// //filter the raw value with a kaman filter to get a more stable value   
float cal::getCurrentF()
{
    //***get the current reading
    float value = analogRead(currentPin);

    //***calculate the current
    float current = (currentSlope * value) + currentYint;
    float kamanValue = PT1GAIN_CURRENT * (current - curFvalue);
    curFvalue = curFvalue + kamanValue;

    return curFvalue;
}

// //filter the raw value with a kaman filter to get a more stable value
float cal::getVoltageF()
{
    //***get the current reading
    float value = analogRead(voltagePin);

    //***calculate the current
    float voltage = (voltageSlope * value) + voltageYint;
    float kamanValue = PT1GAIN_VOLTAGE * (voltage - volFvalue);
    volFvalue = volFvalue + kamanValue;

    return volFvalue;
}

void cal::writeCurrentCal()
{
    EEPROM.writeFloat(60, currentSlope);
    Serial.println("current slope of: " + String(currentSlope) + " written to EEPROM");
    EEPROM.writeFloat(66, currentYint);
    Serial.println("current y intercept of: " + String(currentYint) + " written to EEPROM");
    EEPROM.commit();
    Serial.println("Current calibration EEPROM Write done, board will restart");
    delay(1000);
    ESP.restart(); // hard reset after variant coding
}

void cal::writeVoltageCal()
{
    EEPROM.writeFloat(72, voltageSlope);
    Serial.println("voltage slope of: " + String(voltageSlope) + " written to EEPROM");
    EEPROM.writeFloat(78, voltageYint);
    Serial.println("voltage y intercept of: " + String(voltageYint) + " written to EEPROM");
    EEPROM.commit();
    Serial.println("Voltage calibration EEPROM Write done, board will restart");
    delay(1000);
    ESP.restart(); // hard reset after variant coding
}

void cal::readCal()
{
    currentSlope = EEPROM.readFloat(60);
    Serial.println("current slope of: " + String(currentSlope) + " read from EEPROM");
    currentYint = EEPROM.readFloat(66);
    Serial.println("current y intercept of: " + String(currentYint) + " read from EEPROM");
    voltageSlope = EEPROM.readFloat(72);
    Serial.println("voltage slope of: " + String(voltageSlope) + " read from EEPROM");
    currentYint = EEPROM.readFloat(78);
    Serial.println("voltage intercept of: " + String(voltageYint) + " read from EEPROM");
}

void cal::logDump()
{
    log01.read();
}