# RCMv2

### open in VSCode (using [PlatformIO](https://platformio.org/platformio-ide)), or use the Arduino IDE.

### This example controls a robot with a simple two wheeled drivetrain, and a servo.

https://github.com/rcmgames/RCMv2

version two of RCM robot code

Motor control and communication code has been separated out, improved and made into Arduino libraries available through Arduino's library manager, so there's much less RCM-specific code.
 - https://github.com/joshua-8/JMotor
 - https://github.com/joshua-8/ESP32_easy_wifi_data

Here are some of the new features available in the new version:
 - Better motor control
   - automatic boost to overcome static friction when starting the motor at low speed
   - reduce control dead-zone
   - control actual velocity not just motor power with three point motor curve calibration
   - know approximately how fast your robot can drive based on its current battery level
   - supports many kinds of motor drivers though a common interface (and more can be added easily)
 - Encoders
   - supports quadrature, single channel, and absolute (as5048b) encoders through a common interface (more can be added in the future)
   - precisely control velocity with feedback loop
   - easily drive precise distances with limited acceleration (driving distances is also available without an encoder it just isn't as precise)

Install Driver Station App:

https://github.com/rcmgames/RCMDS-new (recommended)

https://github.com/rcmgames/RCMDS (advanced)
