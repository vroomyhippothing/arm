#ifndef PRESSURE_SENSOR_ANALOG_READ_H
#define PRESSURE_SENSOR_ANALOG_READ_H
#include "pressureSensor.h"
class PressureSensorAnalogRead : public PressureSensor {
protected:
    byte pin;
    float calibration;
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
    /**
     * @brief  the raw value read is multiplied by _calibration value to get pressure
     * @param  _calibration:
     */
    void setCalibration(float _calibration)
    {
        calibration = _calibration;
    }
    /**
     * @brief  the raw value read when the pressure is zero
     * @param  _zero:
     */
    void setZero(float _zero)
    {
        zero = _zero;
    }
};

#endif // PRESSURE_SENSOR_ANALOG_READ_H
