/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <frc/AnalogInput.h>
#include <frc/PWMVictorSPX.h>
#include <frc/TimedRobot.h>
#include <frc/drive/DifferentialDrive.h>
#include <frc/experimental/controller/ControllerRunner.h>
#include <frc/experimental/controller/PIDController.h>

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
    // Set expected range to 0-24 inches; e.g. at 24 inches from object go full
    // forward, at 0 inches from object go full backward.
    m_pidController.SetInputRange(0, 24 * kValueToInches);
    m_pidController.SetPercentTolerance(5);

    // Set setpoint of the PID Controller
    m_pidController.SetReference(kHoldDistance * kValueToInches);

    // Begin PID control
    m_pidRunner.Enable();
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

  frc::AnalogInput m_ultrasonic{kUltrasonicPort};

  frc::PWMVictorSPX m_left{kLeftMotorPort};
  frc::PWMVictorSPX m_right{kRightMotorPort};
  frc::DifferentialDrive m_robotDrive{m_left, m_right};

  frc::experimental::PIDController m_pidController{
      kP, kI, kD, [&] { return m_ultrasonic.GetAverageVoltage(); }};
  frc::experimental::ControllerRunner m_pidRunner{
      m_pidController,
      [&](double output) { m_robotDrive.ArcadeDrive(output, 0); }};
};

#ifndef RUNNING_FRC_TESTS
int main() { return frc::StartRobot<Robot>(); }
#endif
