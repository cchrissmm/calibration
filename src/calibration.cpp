#include <arduino.h>
#include "calibration.h"

// Expecting this as input
//"CALCUR00110,03000" meaning calibrate the current sensor expecting 110mA with no load and 3000mA with load"
//"CALVOL12100" meaning calibrate the voltage sensor expecting 12100mV

// constructor
cal::cal()
{
}

void cal::calStart(String str, int measPin, int trigPin)
{
    if (str.startsWith("CALCUR,")) // starting check for "CALCUR"
    {
        Serial.println("Current meter calibration request received: " + str);
        String y1Str = str.substring(7, 12); // the index positions of the code
        Serial.print("point y1: ");
        int y1 = y1Str.toInt();
        Serial.println(y1);
        if (!y1)
        {
            Serial.println("point y1 contains an error");
            Serial.println(y1Str);
            return;
        }
        Serial.print("measuring x1 (output off state): ");
        analogWrite(trigPin, 0);
        delay(1000);
        int x1 = cal::measure(measPin);
        Serial.println(x1);

        String y2Str = str.substring(13, 18); // the index positions of the code
        Serial.print("point y2: ");
        int y2 = y2Str.toInt();
        Serial.println(y2);
        if (!y2)
        {
            Serial.println("point y2 contains an error");
            Serial.println(y2Str);
            return;
        }
      
        Serial.print("measuring x2 (output on state): ");
        analogWrite(trigPin, 255);
        delay(3000);
        int x2 = cal::measure(measPin);
        Serial.println(x2);
        analogWrite(trigPin, 0);
        cal::fitLinearEqn(x1, y1, x2, y2);
        cal::getCurrent(measPin);
    }
}

//***read the input port a number of times and average it
float cal::measure(int measPin)
{
    float measuredVal;

    for (int i = 0; i < 10; i++)
    {
        measuredVal += analogRead(measPin);
        delay(50);
    }
    float settledVal = measuredVal / 10;

    return settledVal;
}

//***get the linearfunc
void cal::fitLinearEqn(int x1, int y1, int x2, int y2)
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
    delay(1000);
    int currentReading = cal::measure(measPin);

    //***calculate the current
    int current = (slope * currentReading) + yIntercept;
    Serial.println(currentReading);
    Serial.print(" input gives a measured current of (mA): ");
    Serial.println(current);
    return current;
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
