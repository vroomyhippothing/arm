# 1 "C:\\Users\\Joshua\\AppData\\Local\\Temp\\tmphw1ppvn6"
#include <Arduino.h>
# 1 "C:/Users/Joshua/Desktop/arm-git/arm/src/arm.ino"

#include <Arduino.h>
#define EWDmaxWifiSendBufSize 200
#define EWDmaxWifiRecvBufSize 200
#include <ESP32_easy_wifi_data.h>
#include <JMotor.h>

#include "pneumatics/compressorControllers/compressorControllerDigitalWrite.h"
#include "pneumatics/compressorModeConstants.h"
#include "pneumatics/pneumaticBoardController.h"
#include "pneumatics/pneumaticClawController.h"
#include "pneumatics/pressureSensors/pressureSensorAnalogRead.h"
#include "pneumatics/pressureSensors/pressureSensorHX711.h"
#include "pneumatics/valves/analogWriteValve.h"
#include "pneumatics/valves/digitalWriteValve.h"


const byte ONBOARD_LED = 2;
const byte mainVoltageMonitorPin = 36;
const byte pressurizeValvePin = 32;
const byte ventValvePin = 33;
const byte compressorPin = 25;
const byte clawPressureSensorDTPin = 26;
const byte clawPressureSensorSCKPin = 27;
const byte workingPressureSensorDTPin = 14;
const byte workingPressureSensorSCKPin = 12;
const byte storedPressureSensorPin = 39;


const float mainVoltageDACUnitsPerVolt = 380;
const int compressorSetpointHysteresis = 15;
const float compressorDutyLimit = 9;
const float compressorDutyBounds = 4;


bool enabled = false;
byte compressorMode = compressorOff;
float storedPressureSetpoint = 105;
bool clawAuto = false;
bool clawGrabAuto = false;
float clawAutoPressure = 0;
float clawPressurizeVal = 0;
bool clawVentVal = true;


float mainVoltage = 0;
float storedPressure = 0;
float workingPressure = 0;
float clawPressure = 0;
bool compressing = false;
float compressorDuty = 0;


JVoltageCompMeasure<10> mainVoltageComp = JVoltageCompMeasure<10>(mainVoltageMonitorPin, mainVoltageDACUnitsPerVolt);


PressureSensorHX711 clawPressureSensor = PressureSensorHX711(clawPressureSensorDTPin, clawPressureSensorSCKPin, -0.000001, -614000, 1);
PressureSensorHX711 workingPressureSensor = PressureSensorHX711(workingPressureSensorDTPin, workingPressureSensorSCKPin, -0.000001, 232700, 1);
PressureSensorAnalogRead storedPressureSensor = PressureSensorAnalogRead(storedPressureSensorPin, 0.0615, 255);

CompressorControllerDigitalWrite compressorController = CompressorControllerDigitalWrite(compressorPin, HIGH);
PneumaticBoardController pBoard = PneumaticBoardController(compressorController, compressorSetpointHysteresis, compressorDutyLimit, compressorDutyBounds);

AnalogWriteValve clawPressurizeValve = AnalogWriteValve(pressurizeValvePin, false, LOW);
DigitalWriteValve clawVentValve = DigitalWriteValve(ventValvePin, false, LOW);
PneumaticClawController claw = PneumaticClawController(clawPressurizeValve, clawVentValve, 5, 0.1);




bool wasEnabled = false;
unsigned long lastMicros = 0;
unsigned long loopTimeMicros = 0;
inline void Enabled();
inline void Enable();
inline void Disable();
inline void PowerOn();
inline void Always();
void configWifi();
void WifiDataToParse();
void WifiDataToSend();
void setup();
void loop();
#line 75 "C:/Users/Joshua/Desktop/arm-git/arm/src/arm.ino"
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


    pBoard.run(enabled, storedPressure, workingPressure, compressorMode, storedPressureSetpoint);
    compressing = pBoard.isCompressorOn();
    compressorDuty = pBoard.getCompressorDuty();


    claw.run(enabled, clawPressure, clawAuto, clawGrabAuto, clawPressurizeVal, clawVentVal, clawAutoPressure);

    delay(1);
}

void configWifi()
{
    int signalLossTimeout = 125;

    EWD::mode = EWD::Mode::connectToNetwork;
    EWD::routerName = "router";
    EWD::routerPassword = "password";
    EWD::routerPort = 25210;





}

void WifiDataToParse()
{

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
        digitalWrite(ONBOARD_LED, millis() % 500 < 250);
    } else {
        if (!EWD::wifiConnected)
            digitalWrite(ONBOARD_LED, millis() % 1000 <= 100);
        else if (EWD::timedOut())
            digitalWrite(ONBOARD_LED, millis() % 1000 >= 100);
        else
            digitalWrite(ONBOARD_LED, HIGH);
    }
    wasEnabled = enabled;
}