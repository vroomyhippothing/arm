
#include <Arduino.h>
#define EWDmaxWifiSendBufSize 200
#define EWDmaxWifiRecvBufSize 200
#include <ESP32_easy_wifi_data.h> //https://github.com/joshua-8/ESP32_easy_wifi_data
#include <JMotor.h> //https://github.com/joshua-8/JMotor

#include "pneumatics/compressorControllers/compressorControllerDigitalWrite.h"
#include "pneumatics/compressorModeConstants.h"
#include "pneumatics/pneumaticBoardController.h"
#include "pneumatics/pneumaticClawController2.h"
#include "pneumatics/pressureSensors/pressureSensorAnalogRead.h"
#include "pneumatics/pressureSensors/pressureSensorHX711.h"
#include "pneumatics/valves/analogWriteValve.h"
#include "pneumatics/valves/digitalWriteValve.h"

// pins
const byte ONBOARD_LED = 2;
const byte mainVoltageMonitorPin = 36;
const byte rateValvePin = 32;
const byte ventValvePin = 33;
const byte presValvePin = 13;
const byte compressorPin = 25;
const byte clawPressureSensorDTPin = 26;
const byte clawPressureSensorSCKPin = 27;
const byte workingPressureSensorDTPin = 14;
const byte workingPressureSensorSCKPin = 12;
const byte storedPressureSensorPin = 39;

// constants
const float mainVoltageDACUnitsPerVolt = 380;
const int compressorSetpointHysteresis = 15;
const float compressorDutyLimit = 9;
const float compressorDutyBounds = 4;
// (received constants)
float pressureDeadzone = 0;
float autoP = 0;

// received variables
bool enabled = false;
byte compressorMode = compressorOff;
float storedPressureSetpoint = 105;
bool clawAuto = false;
bool clawGrabAuto = false;
float clawAutoPressure = 0;
float clawRateVal = 0;
bool clawPresVal = false;
bool clawVentVal = false;

// sent variables
float mainVoltage = 0;
float storedPressure = 0;
float workingPressure = 0;
float clawPressure = 0;
bool compressing = false;
float compressorDuty = 0;

////////////////////MOTORS////////////////////
JVoltageCompMeasure<10> mainVoltageComp = JVoltageCompMeasure<10>(mainVoltageMonitorPin, mainVoltageDACUnitsPerVolt);

////////////////////PNEUMATICS////////////////////
PressureSensorHX711 clawPressureSensor = PressureSensorHX711(clawPressureSensorDTPin, clawPressureSensorSCKPin, -8.02467E-6, 746000, 1); // dt, sck, calibration, zero, numMeasurements
PressureSensorHX711 workingPressureSensor = PressureSensorHX711(workingPressureSensorDTPin, workingPressureSensorSCKPin, -8.37403E-6, -202100, 1); // dt, sck, calibration, zero, numMeasurements
PressureSensorAnalogRead storedPressureSensor = PressureSensorAnalogRead(storedPressureSensorPin, 0.059, 255); // pin, calibration, zero

CompressorControllerDigitalWrite compressorController = CompressorControllerDigitalWrite(compressorPin, HIGH);
PneumaticBoardController pBoard = PneumaticBoardController(compressorController, compressorSetpointHysteresis, compressorDutyLimit, compressorDutyBounds);

AnalogWriteValve clawRateValve = AnalogWriteValve(rateValvePin, false, LOW); // pin, reverse, disableState
DigitalWriteValve clawPresValve = DigitalWriteValve(presValvePin, false, LOW); // pin, reverse, disableState
DigitalWriteValve clawVentValve = DigitalWriteValve(ventValvePin, false, LOW); // pin, reverse, disableState
PneumaticClawController2 claw = PneumaticClawController2(clawPresValve, clawVentValve, clawRateValve);

////////////////////

// other variables
bool wasEnabled = false;
unsigned long lastMicros = 0;
unsigned long loopTimeMicros = 0;

inline void Enabled()
{
}

inline void Enable()
{
}

inline void Disable()
{
}

inline void PowerOn()
{
    // runs once on robot startup, set pin modes and use begin() if applicable here
    analogReadResolution(12);
    storedPressureSensor.begin();
    workingPressureSensor.begin();
    clawPressureSensor.begin();
}

inline void Always()
{
    unsigned long tempMicros = micros();
    loopTimeMicros = tempMicros - lastMicros;
    lastMicros = tempMicros;
    mainVoltage = mainVoltageComp.getSupplyVoltage();

    storedPressure = storedPressureSensor.getPressure(true);
    workingPressure = workingPressureSensor.getPressure(true);
    clawPressure = clawPressureSensor.getPressure(true);

    // enabling and disabling is handled internally
    pBoard.run(enabled, storedPressure, workingPressure, compressorMode, storedPressureSetpoint);
    compressing = pBoard.isCompressorOn();
    compressorDuty = pBoard.getCompressorDuty();

    // enabling and disabling is handled internally
    claw.run(enabled, clawPressure, clawAuto, clawGrabAuto, clawPresVal, clawVentVal, clawRateVal, clawAutoPressure, pressureDeadzone, autoP);

    delay(1);
}

void configWifi()
{
    int signalLossTimeout = 125;

    EWD::mode = EWD::Mode::connectToNetwork;
    EWD::routerName = "chicken";
    EWD::routerPassword = "bawkbawk";
    EWD::routerPort = 25211;

    // EWD::mode = EWD::Mode::createAP;
    // EWD::APName = "arm";
    // EWD::APPassword = "password";
    // EWD::APPort = 25210;
}

void WifiDataToParse()
{
    // add data to read here: (EWD::recvBl, EWD::recvBy, EWD::recvIn, EWD::recvFl)(bool, byte, int, float)
    enabled = EWD::recvBl();
    compressorMode = EWD::recvBy();
    storedPressureSetpoint = EWD::recvFl();
    clawAuto = EWD::recvBl();
    clawGrabAuto = EWD::recvBl();
    clawAutoPressure = EWD::recvFl();
    clawRateVal = EWD::recvFl();
    clawPresVal = EWD::recvBl();
    clawVentVal = EWD::recvBl();
    pressureDeadzone = EWD::recvFl();
    autoP = EWD::recvFl();
}
void WifiDataToSend()
{
    // add data to send here: (EWD::sendBl(), EWD::sendBy(), EWD::sendIn(), EWD::sendFl())(bool, byte, int, float)
    EWD::sendFl(mainVoltage);
    EWD::sendFl(storedPressure);
    EWD::sendFl(workingPressure);
    EWD::sendFl(clawPressure);
    EWD::sendBl(compressing);
    EWD::sendFl(compressorDuty);
    EWD::sendFl(clawRateValve.getLastSetVal());
    EWD::sendBl(clawPresValve.getLastSetVal());
    EWD::sendBl(clawVentValve.getLastSetVal());
    EWD::sendBl(pBoard.isCompressorOverDutyCycle());
}

////////////////////////////// you don't need to edit below this line ////////////////////

void setup()
{
    Serial.begin(115200);
    pinMode(ONBOARD_LED, OUTPUT);
    PowerOn();
    Disable();
    configWifi();
    EWD::setupWifi(WifiDataToParse, WifiDataToSend);
}

void loop()
{
    EWD::runWifiCommunication();
    if (!EWD::wifiConnected || EWD::timedOut()) {
        enabled = false;
    }
    Always();
    if (enabled && !wasEnabled) {
        Enable();
    }
    if (!enabled && wasEnabled) {
        Disable();
    }
    if (enabled) {
        Enabled();
        digitalWrite(ONBOARD_LED, millis() % 500 < 250); // flash, enabled
    } else {
        if (!EWD::wifiConnected)
            digitalWrite(ONBOARD_LED, millis() % 1000 <= 100); // short flash, wifi connection fail
        else if (EWD::timedOut())
            digitalWrite(ONBOARD_LED, millis() % 1000 >= 100); // long flash, no driver station connected
        else
            digitalWrite(ONBOARD_LED, HIGH); // on, disabled
    }
    wasEnabled = enabled;
}
