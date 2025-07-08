// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/AnalogInput.h>
#include <frc/AnalogTrigger.h>
#include <frc/TimedRobot.h>

/**
 * Analog Trigger snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/digital-input-software.html
 */
class Robot : public frc::TimedRobot {
 public:
  Robot() {
    // Enables 2-bit oversampling
    input.SetAverageBits(2);
    // Sets the trigger to enable at a raw value of 3500, and disable at a value
    // of 1000
    trigger0.SetLimitsRaw(1000, 3500);
    // Sets the trigger to enable at a voltage of 4 volts, and disable at a
    // value of 1.5 volts
    trigger0.SetLimitsVoltage(1.5, 4);
  }

 private:
  // Initializes an AnalogTrigger on port 0
  frc::AnalogTrigger trigger0{0};
  // Initializes an AnalogInput on port 1
  frc::AnalogInput input{1};

  // Initializes an AnalogTrigger using the above input
  frc::AnalogTrigger trigger1{input};
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
