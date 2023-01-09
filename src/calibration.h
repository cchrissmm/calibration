#ifndef CALIBRATION_H
#define CALIBRATION_H

static int p_pt1Gain = 0.5;

class cal {
public:
cal();
void calStart(String str, int measPin, int trigPin);
int measure(int measPin);

private:
int slope;
int yIntercept;
int filteredValue;

void fitLinearEqn(int x1, int y1, int x2, int y2);
int getCurrent(int measPin);
};

#endif 