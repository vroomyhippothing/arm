#ifndef PNEUMATIC_BOARD_CONTROLLER_H
#define PNEUMATIC_BOARD_CONTROLLER_H
// class for controlling our pneumatic board
#include "pressureSensors/pressureSensor.h"
#include <Arduino.h>
class PneumaticBoardController {
protected:
public:
    PneumaticBoardController()
    {
    }
    void run(bool enabled, float storedPressure, float workingPressure, byte compressorMode, float storedPressureSetpoint)
    {
    }
    bool isCompressorOn()
    {
        return false;
    }
    float getCompressorDuty()
    {
        return 0.0; // TODO:
    }
};
#endif // PNEUMATIC_BOARD_CONTROLLER_H
