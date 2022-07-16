#ifndef PRESSURE_SENSOR_HX711_H
#define PRESSURE_SENSOR_HX711_H
#include "pressureSensor.h"
#include <HX711.h> //https://github.com/bogde/HX711
class PressureSensorHX711 : public PressureSensor {
protected:
    byte pinC;
    byte pinD;
    byte numMeasurements;

public:
    HX711 pressureSensor;
    /**
     * @brief  constructor
     * @param  _pinD: hx711 data pin
     * @param  _pinC: hx711 clock pin
     * @param  _calibration: pressurePerDACUnits, the value read through analogRead is multiplied by this value to get pressure, float, default=1.0
     */
    PressureSensorHX711(byte _pinD, byte _pinC, float _calibration = 1.0, int _zero = 0, byte _numMeasurements = 1)
    {
        pinD = _pinD;
        pinC = _pinC;
        pressure = 0;
        numMeasurements = _numMeasurements;
        setCalibration(_calibration);
        setZero(_zero);
    }
    void begin()
    {
        pressureSensor.begin(pinD, pinC); // hx711 DT, SCK
    }
    void run()
    {
        if (pressureSensor.is_ready()) {
            pressure = pressureSensor.get_units(numMeasurements);
        }
    }

    /**
     * @brief  the raw value read is multiplied by _calibration value to get pressure
     * @param  _calibration:
     */
    void setCalibration(float _calibration)
    {
        pressureSensor.set_scale(1.0 / _calibration);
    }
    /**
     * @brief  the raw value read when the pressure is zero
     * @param  _zero:
     */
    void setZero(float _zero)
    {
        pressureSensor.set_offset(_zero);
    }
    /**
     * @brief  how many times does the HX711 library read the loadcell and average the values?
     * @param  _numMeasurements: (byte)
     */
    void setNumMeasurements(byte _numMeasurements)
    {
        numMeasurements = _numMeasurements;
    }
};

#endif // PRESSURE_SENSOR_HX711