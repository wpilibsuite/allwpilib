// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/BuiltInAccelerometer.h>
#include <frc/TimedRobot.h>

/**
 * BuiltIn Accelerometer snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/accelerometers-software.html
 */
class Robot : public frc::TimedRobot {
 public:
  Robot() {
    // Sets the accelerometer to measure between -8 and 8 G's
    m_accelerometer.SetRange(frc::BuiltInAccelerometer::Range::kRange_8G);
  }

  void TeleopPeriodic() override {
    // Gets the current acceleration in the X axis
    m_accelerometer.GetX();
    // Gets the current acceleration in the Y axis
    m_accelerometer.GetY();
    // Gets the current acceleration in the Z axis
    m_accelerometer.GetZ();
  }

 private:
  // Creates an object for the built-in accelerometer
  // Range defaults to +- 8 G's
  frc::BuiltInAccelerometer m_accelerometer;
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
