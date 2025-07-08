// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/BuiltInAccelerometer.h>
#include <frc/TimedRobot.h>
#include <frc/filter/LinearFilter.h>
#include <frc/smartdashboard/SmartDashboard.h>

/**
 * Accelerometer filtering snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/accelerometers-software.html
 */
class Robot : public frc::TimedRobot {
 public:
  void RobotPeriodic() override {
    double XAccel = m_accelerometer.GetX();
    // Get the filtered X acceleration
    double filteredXAccel = m_xAccelFilter.Calculate(XAccel);

    frc::SmartDashboard::PutNumber("X Acceleration", XAccel);
    frc::SmartDashboard::PutNumber("Filtered X Acceleration", filteredXAccel);
  }

 private:
  frc::BuiltInAccelerometer m_accelerometer;
  frc::LinearFilter<double> m_xAccelFilter =
      frc::LinearFilter<double>::MovingAverage(10);
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
