#ifndef PNEUMATIC_CLAW_CONTROLLER_H
#define PNEUMATIC_CLAW_CONTROLLER_H
// a class for controlling our pneumatic claw
// the claw has an AnalogValve for closing and a DigitalValve for opening
#include "valves/analogValve.h"
#include "valves/digitalValve.h"
#include <Arduino.h>
class PneumaticClawController {
protected:
    AnalogValve& pressurizeValve;
    DigitalValve& ventValve;
    float clawAutoPressureHysteresis;
    float clawAutoPressureP;

public:
    PneumaticClawController(AnalogValve& _pressurizeValve, DigitalValve& _ventValve, float _clawAutoPressureHysteresis, float _clawAutoPressureP)
        : pressurizeValve(_pressurizeValve)
        , ventValve(_ventValve)
    {
        clawAutoPressureHysteresis = _clawAutoPressureHysteresis;
        clawAutoPressureP = _clawAutoPressureP;
    }
    void run(bool enabled, float clawPressure, bool clawAuto, bool clawGrabAuto, float clawPressurizeVal, bool clawVentVal, float clawAutoPressure)
    {
        pressurizeValve.setEnable(enabled);
        ventValve.setEnable(enabled);

        if (!clawAuto) {
            pressurizeValve.set(clawPressurizeVal);
            ventValve.set(clawVentVal);
        } else { // claw Auto
            if (!clawGrabAuto) { // open
                pressurizeValve.set(0.0);
                ventValve.set(true);
            } else { // close claw. This is the code that pressurizes the claw to a setpoint.
                if (clawPressure > clawAutoPressure + clawAutoPressureHysteresis) {
                    ventValve.set(true);
                }
                if (clawPressure < clawAutoPressure) {
                    ventValve.set(false);
                }
                pressurizeValve.set(constrain((clawAutoPressure - clawPressure) * clawAutoPressureP, 0, 1));
            }
        }
    }
};
#endif // PNEUMATIC_CLAW_CONTROLLER_H
