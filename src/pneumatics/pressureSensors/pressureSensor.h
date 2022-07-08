#ifndef PRESSURE_SENSOR_H
#define PRESSURE_SENSOR_H
#include <Arduino.h>
class PressureSensor {
protected:
    float pressure;
    float calibration;

public:
    virtual void begin();
    virtual void run();
    /**
     * @brief
     * @note  call run() first to get an updated value
     * @retval
     */
    float getPressure()
    {
        return pressure;
    }
    /**
     * @brief  the raw value read is multiplied by _calibration value to get pressure
     * @param  _calibration:
     */
    void setCalibration(float _calibration)
    {
        calibration = _calibration;
    }
};

#endif // PRESSURE_SENSOR_H
