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
    void run(bool enabled, float clawPressure, bool clawAuto, bool clawGrabAuto, bool clawPresVal, bool clawVentVal, float clawRateVal, float clawAutoPressure, float pressureDeadzone, float autoP)
    {
        pressureDeadzone = abs(pressureDeadzone);
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
                if (clawPressure > clawAutoPressure) {
                    presValve.set(false);
                    if (clawPressure > clawAutoPressure + pressureDeadzone) {
                        ventValve.set(true);
                        // venting, air starts moving at 0.4@50, 0.5@0
                        // TODO: MOVE VALVE COMPENSATION CONSTANTS OUTSIDE OF THIS CLASS
                        rateValve.set(constrain((clawPressure - (clawAutoPressure)) * autoP + (0.5 - 0.1 * clawPressure / 50), 0, 1));
                    } else {
                        rateValve.set(0);
                    }
                } else { // claw pressure <= setpoint
                    ventValve.set(false);
                    if (clawPressure < clawAutoPressure - pressureDeadzone) {
                        presValve.set(true);
                        // pressurizing, air starts moving at 0.54@0, 0.49@50
                        rateValve.set(constrain((clawPressure - (clawAutoPressure)) * -autoP + (0.54 - 0.05 * clawPressure / 50), 0, 1));
                    } else {
                        rateValve.set(0);
                    }
                }
            }
        }
    }
};
#endif // PNEUMATIC_CLAW_CONTROLLER_2_H
