#ifndef DIGITAL_WRITE_VALVE_H
#define DIGITAL_WRITE_VALVE_H
#include "digitalValve.h"
#include <Arduino.h>
class DigitalWriteValve : public DigitalValve {
protected:
    bool enabled;
    byte pin;

public:
    bool disableState = LOW;
    bool reverse;
    /**
     * @brief  constructor, sets pins
     * @param  _pin: what pin to use
     * @param  _reverse: if true: set(true)->LOW and set(false)->HIGH. default: reverse=false
     * @param  _disableState = LOW: when disabled, set pin LOW(default) or HIGH
     */
    DigitalWriteValve(byte _pin, bool _reverse = false, bool _disableState = LOW)
    {
        enabled = false;
        pin = _pin;
        reverse = _reverse;
        disableState = _disableState;
    }
    void set(bool _val)
    {
        if (enabled) {
            if (reverse) {
                _val = !_val;
            }
            digitalWrite(pin, _val);
        }
    }
    void setEnable(bool _enable)
    {
        if (_enable) {
            if (!enabled) {
                // actually enable
                enabled = true;
                pinMode(pin, OUTPUT);
                digitalWrite(pin, disableState);
            }
        } else { // disable
            if (enabled) {
                // actually disable
                enabled = false;
                digitalWrite(pin, disableState);
            }
        }
    }
};

#endif // DIGITAL_WRITE_VALVE_H
