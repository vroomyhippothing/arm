#ifndef COMPRESSOR_CONTROLLER_H
#define COMPRESSOR_CONTROLLER_H
#include <Arduino.h>
class CompressorController {

public:
    virtual void setEnable(bool _enabled);
    virtual void setCompressor(bool _on);
};

#endif // COMPRESSOR_CONTROLLER_H
