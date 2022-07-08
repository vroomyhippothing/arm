
#include <Arduino.h>
#define EWDmaxWifiSendBufSize 200
#define EWDmaxWifiRecvBufSize 200
#include <ESP32_easy_wifi_data.h> //https://github.com/joshua-8/ESP32_easy_wifi_data
#include <JMotor.h> //https://github.com/joshua-8/JMotor

#include "pneumatics/pneumaticBoardController.h"
#include "pneumatics/pneumaticClawController.h"
#include "pneumatics/pressureSensors/pressureSensorAnalogRead.h"
#include "pneumatics/valves/analogWriteValve.h"
#include "pneumatics/valves/digitalWriteValve.h"

// pins
const byte ONBOARD_LED = 2;
const byte mainVoltageMonitorPin = 36;

// constants
const float mainVoltageDACUnitsPerVolt = 380;

// received variables
boolean enabled = false;
byte compressorMode = 2; // off
float storedPressureSetpoint = 105;
boolean clawAuto = false;
boolean clawGrabAuto = false;
float clawAutoPressure = 0;
float clawPressurizeVal = 0;
boolean clawVentVal = true;

// sent variables
float mainVoltage = 0;
float storedPressure = 0;
float workingPressure = 0;
float clawPressure = 0;
boolean compressing = false;
float compressorDuty = 0;

// objects
JVoltageCompMeasure<10> mainVoltageComp = JVoltageCompMeasure<10>(mainVoltageMonitorPin, mainVoltageDACUnitsPerVolt);

AnalogWriteValve clawPressurizeValve = AnalogWriteValve();
DigitalWriteValve clawVentValve = DigitalWriteValve();

PressureSensorAnalogRead storedPressureSensor = PressureSensorAnalogRead(35, 1.0);
PressureSensorAnalogRead workingPressureSensor = PressureSensorAnalogRead(36, 1.0);
// PressureSensor clawPressureSensor = PressureSensor();

PneumaticBoardController pBoard = PneumaticBoardController();
PneumaticClawController claw = PneumaticClawController(clawPressurizeValve, clawVentValve);

// other variables
boolean wasEnabled = false;
unsigned long lastMicros = 0;
unsigned long loopTimeMicros = 0;

void Enabled()
{
}

void Enable()
{
}

void Disable()
{
}

void PowerOn()
{
    // runs once on robot startup, set pin modes and use begin() if applicable here
}

void Always()
{
    unsigned long tempMicros = micros();
    loopTimeMicros = tempMicros - lastMicros;
    lastMicros = tempMicros;
    mainVoltage = mainVoltageComp.getSupplyVoltage();

    // enabling and disabling is handled internally
    pBoard.run(enabled, storedPressure, workingPressure, compressorMode, storedPressureSetpoint);
    compressing = pBoard.isCompressorOn();
    compressorDuty = pBoard.getCompressorDuty();

    // enabling and disabling is handled internally
    claw.run(enabled, clawAuto, clawGrabAuto, clawPressurizeVal, clawVentVal, clawAutoPressure);

    delay(1);
}

void configWifi()
{
    int signalLossTimeout = 500;

    EWD::mode = EWD::Mode::connectToNetwork;
    EWD::routerName = "router";
    EWD::routerPassword = "password";
    EWD::routerPort = 25210;

    // EWD::mode = EWD::Mode::createAP;
    // EWD::APName = "rcm0";
    // EWD::APPassword = "rcmPassword";
    // EWD::APPort = 25210;
}

void WifiDataToParse()
{
    // add data to read here: (EWD::recvBl, EWD::recvBy, EWD::recvIn, EWD::recvFl)(boolean, byte, int, float)
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
    // add data to send here: (EWD::sendBl(), EWD::sendBy(), EWD::sendIn(), EWD::sendFl())(boolean, byte, int, float)
    EWD::sendFl(mainVoltage);
    EWD::sendFl(storedPressure);
    EWD::sendFl(workingPressure);
    EWD::sendFl(clawPressure);
    EWD::sendBl(compressing);
    EWD::sendFl(compressorDuty);
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
