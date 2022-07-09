#ifndef COMPRESSOR_CONTROLLER_DIGITAL_WRITE_H
#define COMPRESSOR_CONTROLLER_DIGITAL_WRITE_H
#include "compressorController.h"
#include <Arduino.h>
class CompressorControllerDigitalWrite : public CompressorController {
protected:
    bool enabled;
    byte pin;

public:
    bool onState;
    /**
     * @brief  constructor, sets pins
     * @param  _pin: what pin to use
     * @param  _onState = HIGH: what pin state turns the compressor on?
     */
    CompressorControllerDigitalWrite(byte _pin, bool _onState = HIGH)
    {
        enabled = false;
        pin = _pin;
        onState = _onState;
    }
    void setCompressor(bool _on)
    {
        if (enabled) {
            digitalWrite(pin, (onState == HIGH) ? _on : !_on);
        }
    }
    void setEnable(bool _enable)
    {
        if (_enable) {
            if (!enabled) {
                // actually enable
                enabled = true;
                pinMode(pin, OUTPUT);
                digitalWrite(pin, !onState);
            }
        } else { // disable
            if (enabled) {
                // actually disable
                enabled = false;
                digitalWrite(pin, !onState);
            }
        }
    }
};

#endif // COMPRESSOR_CONTROLLER_DIGITAL_WRITE_H
