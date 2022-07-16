#ifndef PRESSURE_SENSOR_H
#define PRESSURE_SENSOR_H
#include <Arduino.h>
class PressureSensor {
protected:
    float pressure;

public:
    virtual void begin();
    virtual void run();
    /**
     * @brief
     * @note  call run() first to get an updated value
     * @param _run, if this is true, this function calls run for you
     * @retval
     */
    float getPressure(bool _run = false)
    {
        if (_run) {
            run();
        }
        return pressure;
    }
};

#endif // PRESSURE_SENSOR_H
