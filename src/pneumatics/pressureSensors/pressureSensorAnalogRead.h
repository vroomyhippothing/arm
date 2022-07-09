#ifndef PRESSURE_SENSOR_ANALOG_READ_H
#define PRESSURE_SENSOR_ANALOG_READ_H
#include "pressureSensor.h"
class PressureSensorAnalogRead : public PressureSensor {
protected:
    byte pin;
    int zero;

public:
    /**
     * @brief  constructor
     * @param  _pin: analog capable pin
     * @param  _calibration: pressurePerDACUnits, the value read through analogRead is multiplied by this value to get pressure, float, default=1.0
     */
    PressureSensorAnalogRead(byte _pin, float _calibration = 1.0, int _zero = 0)
    {
        pin = _pin;
        pressure = 0;
        calibration = _calibration;
        zero = _zero;
    }
    void begin()
    {
        pinMode(pin, INPUT);
    }
    void run()
    {
        pressure = (analogRead(pin) - zero) * calibration;
    }
};

#endif // PRESSURE_SENSOR_ANALOG_READ_H
