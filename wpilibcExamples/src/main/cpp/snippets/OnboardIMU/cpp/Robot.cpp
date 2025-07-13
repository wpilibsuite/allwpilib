// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/OnboardIMU.h>
#include <frc/TimedRobot.h>
#include <units/acceleration.h>

/**
 * Onboard IMU snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/accelerometers-software.html
 */
class Robot : public frc::TimedRobot {
 public:
  void TeleopPeriodic() override {
    // Gets the current acceleration in the X axis
    m_accelerometer.GetAccelX();
    // Gets the current acceleration in the Y axis
    m_accelerometer.GetAccelY();
    // Gets the current acceleration in the Z axis
    m_accelerometer.GetAccelZ();
  }

 private:
  // Creates an object for the Systemcore IMU
  frc::OnboardIMU m_accelerometer{frc::OnboardIMU::MountOrientation::kFlat};
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
