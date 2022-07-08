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

public:
    PneumaticClawController(AnalogValve& _pressurizeValve, DigitalValve& _ventValve)
        : pressurizeValve(_pressurizeValve)
        , ventValve(_ventValve)
    {
    }
    void run(bool enabled, bool clawAuto, bool clawGrabAuto, float clawPressurizeVal, bool clawVentVal, float clawAutoPressure)
    {
    }
};
#endif // PNEUMATIC_CLAW_CONTROLLER_H
