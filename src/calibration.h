#ifndef CALIBRATION_H
#define CALIBRATION_H

static float PT1GAIN_CURRENT = 0.1;
static float PT1GAIN_VOLTAGE = 0.1;

class cal {
public:
cal();
    void setup(int cPin, int vPin, int tPin);
    void task20ms();
    void calStart(String str);
    float getCurrentF();
    float getVoltageF();
    void logDump();

private:
    float currentSlope, currentYint, voltageSlope, voltageYint = 0;
    int curFvalue, volFvalue = 0;
    int currentPin, voltagePin, triggerPin;

private:
    void fitCurrentEqn(float x1, float y1, float x2, float y2);
    void fitVoltageEqn(float x1, float y1, float x2, float y2);
    int getCurrent(int measPin);
    float measCal(int measPin);
    void writeCurrentCal();
    void writeVoltageCal();
    void readCal();
};

#endif 