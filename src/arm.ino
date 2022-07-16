
#include <Arduino.h>
#define EWDmaxWifiSendBufSize 200
#define EWDmaxWifiRecvBufSize 200
#include <ESP32_easy_wifi_data.h> //https://github.com/joshua-8/ESP32_easy_wifi_data
#include <JMotor.h> //https://github.com/joshua-8/JMotor

#include "pneumatics/compressorControllers/compressorControllerDigitalWrite.h"
#include "pneumatics/compressorModeConstants.h"
#include "pneumatics/pneumaticBoardController.h"
#include "pneumatics/pneumaticClawController.h"
#include "pneumatics/pressureSensors/pressureSensorAnalogRead.h"
#include "pneumatics/pressureSensors/pressureSensorHX711.h"
#include "pneumatics/valves/analogWriteValve.h"
#include "pneumatics/valves/digitalWriteValve.h"

// pins
const byte ONBOARD_LED = 2;
const byte mainVoltageMonitorPin = 36;
const byte pressurizeValvePin = 32;
const byte ventValvePin = 33;
const byte compressorPin = 25;

// constants
const float mainVoltageDACUnitsPerVolt = 380;
const int compressorSetpointHysteresis = 15;
const float compressorDutyLimit = 9;
const float compressorDutyBounds = 4;

// received variables
bool enabled = false;
byte compressorMode = compressorOff;
float storedPressureSetpoint = 105;
bool clawAuto = false;
bool clawGrabAuto = false;
float clawAutoPressure = 0;
float clawPressurizeVal = 0;
bool clawVentVal = true;

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
PressureSensorAnalogRead storedPressureSensor = PressureSensorAnalogRead(39, 120.0 / 2210, 15); // pin, calibration, zero
PressureSensorAnalogRead workingPressureSensor = PressureSensorAnalogRead(34, 60.0 / 1365, 55); // pin, calibration, zero
// PressureSensorHX711 workingPressureSensor = PressureSensorHX711(19, 18, 0.0001); // dt, sck
PressureSensorAnalogRead clawPressureSensor = PressureSensorAnalogRead(35, 60.0 / 2635, 260); // pin, calibration, zero

CompressorControllerDigitalWrite compressorController = CompressorControllerDigitalWrite(compressorPin, HIGH);
PneumaticBoardController pBoard = PneumaticBoardController(compressorController, compressorSetpointHysteresis, compressorDutyLimit, compressorDutyBounds);

AnalogWriteValve clawPressurizeValve = AnalogWriteValve(pressurizeValvePin, false, LOW); // pin, reverse, disableState
DigitalWriteValve clawVentValve = DigitalWriteValve(ventValvePin, false, LOW); // pin, reverse, disableState
PneumaticClawController claw = PneumaticClawController(clawPressurizeValve, clawVentValve, 5, 0.1);

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
    claw.run(enabled, clawPressure, clawAuto, clawGrabAuto, clawPressurizeVal, clawVentVal, clawAutoPressure);

    delay(1);
}

void configWifi()
{
    int signalLossTimeout = 125;

    // EWD::mode = EWD::Mode::connectToNetwork;
    // EWD::routerName = "router";
    // EWD::routerPassword = "password";
    // EWD::routerPort = 25210;

    EWD::mode = EWD::Mode::createAP;
    EWD::APName = "arm";
    EWD::APPassword = "password";
    EWD::APPort = 25210;
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
    clawPressurizeVal = EWD::recvFl();
    clawVentVal = EWD::recvBl();
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
    EWD::sendFl(clawPressurizeValve.getLastSetVal());
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
