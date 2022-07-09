#ifndef ANALOG_VALVE_H
#define ANALOG_VALVE_H
#include <Arduino.h>
class AnalogValve {
public:
    virtual void setEnable(bool _enabled);
    virtual void set(float _val);
};
#endif // ANALOG_VALVE_H
