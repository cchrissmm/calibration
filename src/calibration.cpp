#include <arduino.h>
#include "calibration.h"

// Expecting this as input
//"CALCUR00110,03000" meaning calibrate the current sensor expecting 110mA with no load and 3000mA with load"
//"CALVOL12100" meaning calibrate the voltage sensor expecting 12100mV

void calStart(String str, int measPin, int trigPin)
{
    if (str.startsWith("CALCUR")) // starting check for "CALCUR"
    {
        Serial.println("Current meter calibration request received: " + str);
        String x1Str = str.substring(7, 11); // the index positions of the code
        Serial.print("point 1: ");
        Serial.println(x1Str);
        String x2Str = str.substring(12, 16); // the index positions of the code
        Serial.print("point 2: ");
        Serial.println(x1Str);

        int x1 = x1Str.toInt();
        if (x1)
        {
            Serial.println("point x1 looks good");
        }
        else
        {
            Serial.println("point x1 contains an error");
            return;
        }

        int x2 = x2Str.toInt();
        if (x2)
        {
            Serial.println("point x2 looks good");
        }
        else
        {
            Serial.println("point x2 contains an error");
            return;
        }

        Serial.print("measuring y1 (output off state): ");
        analogWrite(trigPin, 0);
        delay(1000);
        int y1 = measure(measPin);
        Serial.println(y1);

        Serial.print("measuring y2 (output on state): ");
        analogWrite(trigPin, 255);
        delay(1000);
        int y2 = measure(measPin);
        Serial.println(y2);

        fitLinearEqn(x1, y1, x2, y2);
        Serial.print("slope is ");
       Serial.println(slope);
        Serial.print("y int is ");
       Serial.println(yIntercept);
    }  
}

//***read the input port a number of times and average it
float measure(int measPin)
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
void fitLinearEqn(int x1, int y1, int x2, int y2)
{
    // calculate the slope of the line
    slope = (y2 - y1) / (x2 - x1);

    // calculate the y-intercept of the line
    yIntercept = y1 - (slope * x1);
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
