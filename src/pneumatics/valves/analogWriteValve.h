#ifndef ANALOG_WRITE_VALVE_H
#define ANALOG_WRITE_VALVE_H
#include "analogValve.h"
#include <Arduino.h>
class AnalogWriteValve : public AnalogValve {
protected:
    bool enabled;
    byte pin;
    float lastSetVal;

public:
    /**
     * @brief  change PWM_RANGE to match the range of analogWrite (change after using analogWriteResolution)
     */
    int PWM_RANGE = 255;
    bool disableState = LOW;
    bool reverse;
    /**
     * @brief  constructor, sets pins
     * @note   make sure to check which pins on your board are PWM capable
     * @param  _pin: what pin to use
     * @param  _reverse: if true: set(1.0)->LOW and set(0.1)->HIGH. default: reverse=false
     * @param  _disableState = LOW: when disabled, set pin LOW(default) or HIGH
     */
    AnalogWriteValve(byte _pin, bool _reverse = false, bool _disableState = LOW)
    {
        enabled = false;
        pin = _pin;
        reverse = _reverse;
        disableState = _disableState;
        lastSetVal = 0.0;
    }
    void set(float _val)
    {
        lastSetVal = _val;
        if (enabled) {
            if (reverse) {
                _val = 1.0 - _val;
            }
            int val = constrain(_val * PWM_RANGE, 0, PWM_RANGE);
            analogWrite(pin, val);
        }
    }
    void setEnable(bool _enable)
    {
        if (_enable) {
            if (!enabled) {
                // actually enable
                enabled = true;
                analogWrite(pin, !disableState ? 0 : PWM_RANGE);
            }
        } else { // disable
            if (enabled) {
                // actually disable
                enabled = false;
                analogWrite(pin, !disableState ? 0 : PWM_RANGE);
            }
        }
    }
    float getLastSetVal()
    {
        return lastSetVal;
    }
};
#endif // ANALOG_WRITE_VALVE_H
