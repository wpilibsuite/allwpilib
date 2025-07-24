// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/OnboardIMU.h>
#include <frc/TimedRobot.h>
#include <frc/filter/LinearFilter.h>
#include <frc/smartdashboard/SmartDashboard.h>
#include <units/acceleration.h>

/**
 * Accelerometer filtering snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/accelerometers-software.html
 */
class Robot : public frc::TimedRobot {
 public:
  void RobotPeriodic() override {
    units::meters_per_second_squared_t XAccel = m_accelerometer.GetAccelX();
    // Get the filtered X acceleration
    units::meters_per_second_squared_t filteredXAccel =
        m_xAccelFilter.Calculate(XAccel);

    frc::SmartDashboard::PutNumber("X Acceleration", XAccel.value());
    frc::SmartDashboard::PutNumber("Filtered X Acceleration",
                                   filteredXAccel.value());
  }

 private:
  frc::OnboardIMU m_accelerometer{frc::OnboardIMU::MountOrientation::kFlat};
  frc::LinearFilter<units::meters_per_second_squared_t> m_xAccelFilter =
      frc::LinearFilter<units::meters_per_second_squared_t>::MovingAverage(10);
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
