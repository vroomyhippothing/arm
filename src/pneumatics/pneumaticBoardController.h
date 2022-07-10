#ifndef PNEUMATIC_BOARD_CONTROLLER_H
#define PNEUMATIC_BOARD_CONTROLLER_H
// class for controlling our pneumatic board
#include "compressorControllers/compressorController.h"
#include "compressorModeConstants.h"
#include "pneumatics/valves/digitalWriteValve.h"
#include <Arduino.h>
class PneumaticBoardController {
protected:
    bool compressing;
    float compressorDuty;
    CompressorController& compressorController;

    int compressorSetpointHysteresis;
    unsigned long lastDutyCycleCalculation;

public:
    PneumaticBoardController(CompressorController& _compressorController, int _compressorSetpointHysteresis /*dutyCycle limit and other settings?*/)
        : compressorController(_compressorController)
    {
        compressorSetpointHysteresis = _compressorSetpointHysteresis;
        compressing = false;
        compressorDuty = 0.0;
        lastDutyCycleCalculation = 0;
    }
    void run(bool enabled, float storedPressure, float workingPressure, byte compressorMode, float storedPressureSetpoint)
    {
        compressorController.setEnable(enabled); // kind of redundant, since compressing probably shouldn't be true when disabled
        switch (compressorMode) {
        case compressorOverride:
            compressing = enabled;
            break;
        case compressorNormal:
            if (!enabled) {
                compressing = false;
            } else { // enabled
                if (storedPressure < storedPressureSetpoint - compressorSetpointHysteresis) {
                    compressing = true;
                } else if (storedPressure >= storedPressureSetpoint) {
                    compressing = false;
                }
            }
            break;
        default: // compressorOff and any other value
            compressing = false;
            break;
        }

        // duty cycle calculation
        if (millis() - lastDutyCycleCalculation > 100) {
            if (enabled && compressing) {
                compressorDuty += (1.0 - compressorDuty) * (millis() - lastDutyCycleCalculation) / 1000.0 / (55.0 * 60);
            } else {
                compressorDuty += (0.0 - compressorDuty) * (millis() - lastDutyCycleCalculation) / 1000.0 / (55.0 * 60);
            }
            lastDutyCycleCalculation = millis();
        }

        compressorController.setCompressor(compressing);
    }
    bool isCompressorOn()
    {
        return compressing;
    }
    float getCompressorDuty()
    {
        return compressorDuty;
    }
};
#endif // PNEUMATIC_BOARD_CONTROLLER_H
