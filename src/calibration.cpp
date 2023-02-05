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

void cal::setup(int cPin, int vPin) {
    this -> currentPin = cPin;
    this -> voltagePin = vPin;
}

void cal::task20ms() {
    cal::getCurrentFilt(currentPin);
}

void cal::calStart(String str, int measPin, int trigPin)
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
        analogWrite(trigPin, 0);
        float x1 = cal::measCal(measPin);
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
        analogWrite(trigPin, 255);
        float x2 = cal::measCal(measPin);
        Serial.println(x2);
        
        analogWrite(trigPin, 0);
        cal::fitLinearEqn(x1, y1, x2, y2);
        cal::getCurrent(measPin);

        analogWrite(trigPin, 255);
        cal::fitLinearEqn(x1, y1, x2, y2);
        cal::getCurrent(measPin);
        analogWrite(trigPin, 0);
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
void cal::fitLinearEqn(float x1, float y1, float x2, float y2)
{
    // calculate the slope of the line
    slope = (y2 - y1) / (x2 - x1);
    Serial.print("slope is ");
    Serial.println(slope);

    // calculate the y-intercept of the line
    yIntercept = y1 - (slope * x1);
    Serial.print("y int is ");
    Serial.println(yIntercept);
}

int cal::getCurrent(int measPin)
{
    //***get the current reading
    float value = cal::measCal(measPin);

    //***calculate the current
    float current = (slope * value) + yIntercept;
    Serial.print("input: ");
    Serial.println(value);
    Serial.print("gives a measured current of (mA): ");
    Serial.println(current);
    return current;
}

// //filter the raw value with a kaman filter to get a more stable value   
float cal::getCurrentFilt(int measPin)
{
    //***get the current reading
    float value = cal::measCal(measPin);

    //***calculate the current
    float current = (slope * value) + yIntercept;
    float kamanValue = PT1GAIN_CURRENT * (current - filteredValue);
    filteredValue = filteredValue + kamanValue;
    String log = String(current) + "," + String(filteredValue);
    log01.data(log);
    return filteredValue;
}


// void writeMeterCal(int startPos)
// {
//     EEPROM.writeInt(startPos, data);
//     Serial.println("Eeprom index: ");
//     Serial.println(startPos);
//     Serial.println("written with: ");
//     Serial.println(data);

//     EEPROM.commit();
//     Serial.println("Calibration EEPROM Write done, board will restart");
//     delay(1000);
//     ESP.restart(); // hard reset after variant coding
// }

void cal::logDump()
{
    log01.read();
}