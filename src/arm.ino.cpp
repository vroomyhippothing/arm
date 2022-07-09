# 1 "C:\\Users\\Joshua\\AppData\\Local\\Temp\\tmpfqlab157"
#include <Arduino.h>
# 1 "C:/Users/Joshua/Desktop/arm-git/arm/src/arm.ino"

#include <Arduino.h>
#define EWDmaxWifiSendBufSize 200
#define EWDmaxWifiRecvBufSize 200
#include <ESP32_easy_wifi_data.h>
#include <JMotor.h>

#include "pneumatics/compressorControllers/compressorControllerDigitalWrite.h"
#include "pneumatics/pneumaticBoardController.h"
#include "pneumatics/pneumaticClawController.h"
#include "pneumatics/pressureSensors/pressureSensorAnalogRead.h"
#include "pneumatics/valves/analogWriteValve.h"
#include "pneumatics/valves/digitalWriteValve.h"


const byte ONBOARD_LED = 2;
const byte mainVoltageMonitorPin = 36;
const byte pressurizeValvePin = 24;
const byte ventValvePin = 25;


const float mainVoltageDACUnitsPerVolt = 380;
const byte compressorOff = 2;
const byte compressorNormal = 1;
const byte compressorOverride = 0;


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

PressureSensorAnalogRead storedPressureSensor = PressureSensorAnalogRead(35, 1.0, 0);
PressureSensorAnalogRead workingPressureSensor = PressureSensorAnalogRead(36, 1.0);
PressureSensorAnalogRead clawPressureSensor = PressureSensorAnalogRead(25, 1.0);

CompressorControllerDigitalWrite compressorController = CompressorControllerDigitalWrite(34, HIGH);

PneumaticBoardController pBoard = PneumaticBoardController(compressorController);

AnalogWriteValve clawPressurizeValve = AnalogWriteValve(pressurizeValvePin, false, LOW);
DigitalWriteValve clawVentValve = DigitalWriteValve(ventValvePin, false, LOW);

PneumaticClawController claw = PneumaticClawController(clawPressurizeValve, clawVentValve);


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
#line 66 "C:/Users/Joshua/Desktop/arm-git/arm/src/arm.ino"
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

}

inline void Always()
{
    unsigned long tempMicros = micros();
    loopTimeMicros = tempMicros - lastMicros;
    lastMicros = tempMicros;
    mainVoltage = mainVoltageComp.getSupplyVoltage();


    pBoard.run(enabled, storedPressure, workingPressure, compressorMode, storedPressureSetpoint);
    compressing = pBoard.isCompressorOn();
    compressorDuty = pBoard.getCompressorDuty();


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