// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/OnboardIMU.h>
#include <frc/TimedRobot.h>
#include <frc/smartdashboard/SmartDashboard.h>
#include <units/acceleration.h>

/**
 * Collision detection snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/accelerometers-software.html
 */
class Robot : public frc::TimedRobot {
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

    frc::SmartDashboard::PutNumber("X Jerk", xJerk.value());
    frc::SmartDashboard::PutNumber("Y Jerk", yJerk.value());
  }

 private:
  units::meters_per_second_squared_t m_prevXAccel = 0.0_mps_sq;
  units::meters_per_second_squared_t m_prevYAccel = 0.0_mps_sq;
  frc::OnboardIMU m_accelerometer{frc::OnboardIMU::MountOrientation::kFlat};
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
