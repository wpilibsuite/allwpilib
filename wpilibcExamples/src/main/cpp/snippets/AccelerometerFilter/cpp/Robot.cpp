// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/hardware/imu/OnboardIMU.hpp>
#include <wpi/math/filter/LinearFilter.hpp>
#include <wpi/opmode/TimedRobot.hpp>
#include <wpi/smartdashboard/SmartDashboard.hpp>
#include <wpi/units/acceleration.hpp>

/**
 * Accelerometer filtering snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/accelerometers-software.html
 */
class Robot : public wpi::TimedRobot {
 public:
  void RobotPeriodic() override {
    wpi::units::meters_per_second_squared_t XAccel = m_accelerometer.GetAccelX();
    // Get the filtered X acceleration
    wpi::units::meters_per_second_squared_t filteredXAccel =
        m_xAccelFilter.Calculate(XAccel);

    wpi::SmartDashboard::PutNumber("X Acceleration", XAccel.value());
    wpi::SmartDashboard::PutNumber("Filtered X Acceleration",
                                   filteredXAccel.value());
  }

 private:
  wpi::OnboardIMU m_accelerometer{wpi::OnboardIMU::MountOrientation::kFlat};
  wpi::math::LinearFilter<wpi::units::meters_per_second_squared_t> m_xAccelFilter =
      wpi::math::LinearFilter<wpi::units::meters_per_second_squared_t>::MovingAverage(10);
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
