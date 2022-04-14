// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/AnalogInput.h>
#include <frc/TimedRobot.h>
#include <frc/controller/PIDController.h>
#include <frc/drive/DifferentialDrive.h>
#include <frc/filter/MedianFilter.h>
#include <frc/motorcontrol/PWMSparkMax.h>

/**
 * This is a sample program demonstrating how to use an ultrasonic sensor and
 * proportional control to maintain a set distance from an object.
 */
class Robot : public frc::TimedRobot {
 public:
  /**
   * Drives the robot a set distance from an object using PID control and the
   * ultrasonic sensor.
   */
  void TeleopInit() override {
    // Set setpoint of the PID Controller
    m_pidController.SetSetpoint(kHoldDistance * kValueToInches);
  }

  void TeleopPeriodic() override {
    double output =
        m_pidController.Calculate(m_filter.Calculate(m_ultrasonic.GetValue()));
    m_robotDrive.ArcadeDrive(output, 0);
  }

 private:
  // Distance in inches the robot wants to stay from an object
  static constexpr int kHoldDistance = 12;

  // Factor to convert sensor values to a distance in inches
  static constexpr double kValueToInches = 0.125;

  // proportional speed constant
  static constexpr double kP = 7.0;

  // integral speed constant
  static constexpr double kI = 0.018;

  // derivative speed constant
  static constexpr double kD = 1.5;

  static constexpr int kLeftMotorPort = 0;
  static constexpr int kRightMotorPort = 1;
  static constexpr int kUltrasonicPort = 0;

  // median filter to discard outliers; filters over 5 samples
  frc::MedianFilter<double> m_filter{5};

  frc::AnalogInput m_ultrasonic{kUltrasonicPort};

  frc::PWMSparkMax m_left{kLeftMotorPort};
  frc::PWMSparkMax m_right{kRightMotorPort};
  frc::DifferentialDrive m_robotDrive{m_left, m_right};

  frc2::PIDController m_pidController{kP, kI, kD};
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
