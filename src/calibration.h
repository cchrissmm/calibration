#ifndef CALIBRATION_H
#define CALIBRATION_H

class cal {
public:
cal();
void calStart(String str, int measPin, int trigPin);

private:
int slope;
int yIntercept;
float measure(int measPin);
void fitLinearEqn(int x1, int y1, int x2, int y2);
int getCurrent(int measPin);

};

#endif 