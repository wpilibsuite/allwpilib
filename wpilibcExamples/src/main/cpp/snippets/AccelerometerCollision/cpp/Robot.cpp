// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/hardware/imu/OnboardIMU.hpp>
#include <wpi/opmode/TimedRobot.hpp>
#include <wpi/smartdashboard/SmartDashboard.hpp>
#include <wpi/units/acceleration.hpp>

/**
 * Collision detection snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/accelerometers-software.html
 */
class Robot : public wpi::TimedRobot {
 public:
  void RobotPeriodic() override {
    // Gets the current accelerations in the X and Y directions
    auto xAccel = m_accelerometer.GetAccelX();
    auto yAccel = m_accelerometer.GetAccelY();
    // Calculates the jerk in the X and Y directions
    auto xJerk = (xAccel - m_prevXAccel) / GetPeriod();
    auto yJerk = (yAccel - m_prevYAccel) / GetPeriod();
    m_prevXAccel = xAccel;
    m_prevYAccel = yAccel;

    wpi::SmartDashboard::PutNumber("X Jerk", xJerk.value());
    wpi::SmartDashboard::PutNumber("Y Jerk", yJerk.value());
  }

 private:
  wpi::units::meters_per_second_squared_t m_prevXAccel = 0.0_mps_sq;
  wpi::units::meters_per_second_squared_t m_prevYAccel = 0.0_mps_sq;
  wpi::OnboardIMU m_accelerometer{wpi::OnboardIMU::MountOrientation::kFlat};
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
