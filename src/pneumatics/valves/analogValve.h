#ifndef ANALOG_VALVE_H
#define ANALOG_VALVE_H
#include <Arduino.h>
class AnalogValve {
protected:
    bool enabled;

public:
    virtual void setEnabled(bool _enabled);
    virtual void set(float _val);
};
#endif // ANALOG_VALVE_H
