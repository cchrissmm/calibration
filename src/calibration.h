#ifndef CALIBRATION_H
#define CALIBRATION_H

static float p_pt1Gain = 0.1;

class cal {
public:
cal();
void calStart(String str, int measPin, int trigPin);
float measure(int measPin);
void logDump();

private:
float slope = 0;
float yIntercept = 0;
int filteredValue = 0;

void fitLinearEqn(float x1, float y1, float x2, float y2);
int getCurrent(int measPin);
float measCal(int measPin);
};

#endif 