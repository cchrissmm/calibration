#ifndef CALIBRATION_H
#define CALIBRATION_H

int slope;
int yIntercept;

void calStart(String str, int measPin, int trigPin);
float measure(int measPin);
void fitLinearEqn(int x1, int y1, int x2, int y2);

#endif 