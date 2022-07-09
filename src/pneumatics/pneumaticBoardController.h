#ifndef PNEUMATIC_BOARD_CONTROLLER_H
#define PNEUMATIC_BOARD_CONTROLLER_H
// class for controlling our pneumatic board
#include "compressorControllers/compressorController.h"
#include <Arduino.h>
class PneumaticBoardController {
protected:
    bool compressing;
    float compressorDuty;
    CompressorController& compressorController;

public:
    PneumaticBoardController(CompressorController& _compressorController /*compressorController, dutyCycle limit and other settings?*/)
        : compressorController(_compressorController)
    {
        compressing = false;
        compressorDuty = 0.0;
    }
    void run(bool enabled, float storedPressure, float workingPressure, byte compressorMode, float storedPressureSetpoint)
    {
        compressorController.setEnable(enabled);
        compressing = true;
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
