// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/framework/TimedRobot.hpp>
#include <wpi/hardware/imu/OnboardIMU.hpp>
#include <wpi/math/geometry/Rotation2d.hpp>
#include <wpi/units/acceleration.hpp>
#include <wpi/units/angle.hpp>

/**
 * Onboard IMU snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/accelerometers-software.html
 */
class Robot : public wpi::TimedRobot {
 public:
  void TeleopPeriodic() override {
    // Gets the current acceleration in the X axis
    m_IMU.GetAccelX();
    // Gets the current acceleration in the Y axis
    m_IMU.GetAccelY();
    // Gets the current acceleration in the Z axis
    m_IMU.GetAccelZ();

    // Gets the current angle in the X axis
    m_IMU.GetAngleX();
    // Gets the current angle in the Y axis
    m_IMU.GetAngleY();
    // Gets the current angle in the Z axis
    m_IMU.GetAngleZ();

    // Gets the current angle as a Rotation2D
    m_IMU.GetRotation2d();
  }

 private:
  // Creates an object for the Systemcore IMU
  wpi::OnboardIMU m_IMU{wpi::OnboardIMU::MountOrientation::kFlat};
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
