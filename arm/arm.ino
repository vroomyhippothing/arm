
#include <Arduino.h>
#include <ESP32_easy_wifi_data.h> //https://github.com/joshua-8/ESP32_easy_wifi_data
#include <JMotor.h> //https://github.com/joshua-8/JMotor
const byte ONBOARD_LED = 2;
boolean enabled = false;
boolean wasEnabled = false;
// set up motors and anything else you need here

void Enabled()
{
}

void Enable()
{
  // turn on outputs
}

void Disable()
{
  // shut off all outputs
}

void PowerOn()
{
  // runs once on robot startup, set pin modes and use begin() if applicable here
}

void Always()
{
  // always runs if void loop is running, JMotor run() functions should be put here
  // (but only the "top level", for example if you call drivetrainController.run() you shouldn't also call motorController.run())

  delay(1);
}

void configWifi()
{

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
  enabled = EWD::recvBl();
  // add data to read here: (EWD::recvBl, EWD::recvBy, EWD::recvIn, EWD::recvFl)(boolean, byte, int, float)
}
void WifiDataToSend()
{
  EWD::sendFl(2.521);
  // add data to send here: (EWD::sendBl(), EWD::sendBy(), EWD::sendIn(), EWD::sendFl())(boolean, byte, int, float)
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
