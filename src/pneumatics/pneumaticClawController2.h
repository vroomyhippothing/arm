#ifndef PNEUMATIC_CLAW_CONTROLLER_2_H
#define PNEUMATIC_CLAW_CONTROLLER_2_H
// a class for controlling our pneumatic claw
// the claw has an DigitalValve for closing, a DigitalValve for opening, and an AnalogValve for limiting the claw's speed
#include "valves/analogValve.h"
#include "valves/digitalValve.h"
#include <Arduino.h>
class PneumaticClawController2 {
protected:
    AnalogValve& rateValve;
    DigitalValve& ventValve;
    DigitalValve& presValve;

public:
    PneumaticClawController2(DigitalValve& _presValve, DigitalValve& _ventValve, AnalogValve& _rateValve)
        : presValve(_presValve)
        , ventValve(_ventValve)
        , rateValve(_rateValve)
    {
    }
    void run(bool enabled, float clawPressure, bool clawAuto, bool clawGrabAuto, bool clawPresVal, bool clawVentVal, float clawRateVal, float clawAutoPressure)
    {
        presValve.setEnable(enabled);
        ventValve.setEnable(enabled);
        rateValve.setEnable(enabled);
        if (!clawAuto) {
            presValve.set(clawPresVal);
            ventValve.set(clawVentVal);
            rateValve.set(clawRateVal);
        } else { // claw Auto
            if (!clawGrabAuto) { // open
                rateValve.set(1.0);
                presValve.set(false);
                ventValve.set(true);
            } else { // close claw. This is the code that pressurizes the claw to a setpoint.
                rateValve.set(0.8);
                presValve.set(true);
                ventValve.set(false);
            }
        }
    }
};
#endif // PNEUMATIC_CLAW_CONTROLLER_2_H
