// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/BuiltInAccelerometer.h>
#include <frc/TimedRobot.h>
#include <frc/smartdashboard/SmartDashboard.h>

/**
 * Collision detection snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/accelerometers-software.html
 */
class Robot : public frc::TimedRobot {
 public:
  void RobotPeriodic() override {
    // Gets the current accelerations in the X and Y directions
    double xAccel = m_accelerometer.GetX();
    double yAccel = m_accelerometer.GetY();
    // Calculates the jerk in the X and Y directions
    // Divides by .02 because default loop timing is 20ms
    double xJerk = (xAccel - m_prevXAccel) / 0.02;
    double yJerk = (yAccel - m_prevYAccel) / 0.02;
    m_prevXAccel = xAccel;
    m_prevYAccel = yAccel;

    frc::SmartDashboard::PutNumber("X Jerk", xJerk);
    frc::SmartDashboard::PutNumber("Y Jerk", yJerk);
  }

 private:
  double m_prevXAccel = 0.0;
  double m_prevYAccel = 0.0;
  frc::BuiltInAccelerometer m_accelerometer;
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
