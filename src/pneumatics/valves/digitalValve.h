#ifndef DIGITAL_VALVE_H
#define DIGITAL_VALVE_H
#include <Arduino.h>
class DigitalValve {
public:
    virtual void setEnable(bool _enabled);
    virtual void set(bool _val);
};
#endif // DIGITAL_VALVE_H
