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
    float compressorDutyLimit;
    float compressorDutyBounds;
    unsigned long lastDutyCycleCalculation;
    bool overDutyCycle;

public:
    PneumaticBoardController(CompressorController& _compressorController, int _compressorSetpointHysteresis, float _compressorDutyLimit, float _compressorDutyBounds)
        : compressorController(_compressorController)
    {
        compressorSetpointHysteresis = _compressorSetpointHysteresis;
        compressing = false;
        compressorDuty = 0.0;
        lastDutyCycleCalculation = 0;
        overDutyCycle = false;
        compressorDutyLimit = _compressorDutyLimit;
        compressorDutyBounds = _compressorDutyBounds;
    }
    void run(bool enabled, float storedPressure, float workingPressure, byte compressorMode, float storedPressureSetpoint)
    {

        // duty cycle calculation
        if (millis() - lastDutyCycleCalculation > 100) {
            if (enabled && compressing) {
                compressorDuty += (1.0 - compressorDuty) * (millis() - lastDutyCycleCalculation) / 1000.0 / (55.0 * 60);
            } else {
                compressorDuty += (0.0 - compressorDuty) * (millis() - lastDutyCycleCalculation) / 1000.0 / (55.0 * 60);
            }
            lastDutyCycleCalculation = millis();
        }

        if (compressorDuty * 100.0 > compressorDutyLimit + compressorDutyBounds) {
            overDutyCycle = true;
        }
        if (compressorDuty * 100.0 < compressorDutyLimit - compressorDutyBounds) {
            overDutyCycle = false;
        }

        compressorController.setEnable(enabled); // kind of redundant, since compressing probably shouldn't be true when disabled
        switch (compressorMode) {
        case compressorOverride:
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
        case compressorNormal:
            if (!enabled) {
                compressing = false;
            } else { // enabled
                if (overDutyCycle) {
                    compressing = false;
                } else if (storedPressure < storedPressureSetpoint - compressorSetpointHysteresis) {
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
    bool isCompressorOverDutyCycle()
    {
        return overDutyCycle;
    }
};
#endif // PNEUMATIC_BOARD_CONTROLLER_H
