#ifndef CALIBRATION_H
#define CALIBRATION_H

static float PT1GAIN_CURRENT = 0.1;

class cal {
public:
cal();
void setup(int cPin, int vPin);
void task20ms();
void calStart(String str, int measPin, int trigPin);
float getCurrentFilt(int measPin);
void logDump();

private:
float slope = 0;
float yIntercept = 0;
int filteredValue = 0;

void fitLinearEqn(float x1, float y1, float x2, float y2);
int getCurrent(int measPin);
float measCal(int measPin);
int currentPin;
int voltagePin;
};

#endif 