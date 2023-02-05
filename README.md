# calibration
This is a routine to calibrate the onbaord ACS712 based current meter and the voltage divider based voltage meter. 

# usage
include the header

`#include <calibration.h>`

create an instance of the class:

`cal cal01;`

call the calibration method:

`cal01.calStart(str, GPIOmeas, GPIOtrig);`

the str contains the measured calibration values and a prefix to indicate if the current meter or volage meter is being calibrated

`CALCUR,00070,04670` 

meaning calibrate the current sensor expecting 70mA with no load and 4670mA with load"

`CALVOL,12100` 

meaning calibrate the voltage sensor expecting 12100mV

GPIOMeas is the input pin connected to the sensor

GPIOtrig is the output pin connected to the load trigger

typically app01 - 03 would be used 

# serial
its easy to trigger it from a erial input as

`if (str.startsWith("CALCUR")) // starting check for "VAR"
    {
      Serial.println("Current meter calibration requested");
      cal01.calStart(str, GPIOmeas, GPIOtrig);`
    }