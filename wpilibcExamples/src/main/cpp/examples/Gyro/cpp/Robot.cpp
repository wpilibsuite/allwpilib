// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>

#include "wpi/drive/DifferentialDrive.hpp"
#include "wpi/driverstation/Joystick.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/hardware/imu/OnboardIMU.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"

/**
 * This is a sample program to demonstrate how to use a gyro sensor to make a
 * robot drive straight. This program uses a joystick to drive forwards and
 * backwards while the gyro is used for direction keeping.
 */
class Robot : public wpi::TimedRobot {
 public:
  Robot() {
    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    right.SetInverted(true);

    wpi::util::SendableRegistry::AddChild(&drive, &left);
    wpi::util::SendableRegistry::AddChild(&drive, &right);
  }

  /**
   * The motor velocity is set from the joystick while the DifferentialDrive
   * turning value is assigned from the error between the setpoint and the gyro
   * angle.
   */
  void TeleopPeriodic() override {
    double turningValue =
        (kAngleSetpoint - imu.GetRotation2d().Degrees().value()) * kP;
    drive.ArcadeDrive(-joystick.GetY(), -turningValue);
  }

 private:
  static constexpr double kAngleSetpoint = 0.0;
  static constexpr double kP = 0.005;  // Proportional turning constant

  static constexpr int kLeftMotorPort = 0;
  static constexpr int kRightMotorPort = 1;
  static constexpr wpi::OnboardIMU::MountOrientation kIMUMountOrientation =
      wpi::OnboardIMU::FLAT;
  static constexpr int kJoystickPort = 0;

  wpi::PWMSparkMax left{kLeftMotorPort};
  wpi::PWMSparkMax right{kRightMotorPort};
  wpi::DifferentialDrive drive{
      [&](double output) { left.SetThrottle(output); },
      [&](double output) { right.SetThrottle(output); }};

  wpi::OnboardIMU imu{kIMUMountOrientation};
  wpi::Joystick joystick{kJoystickPort};
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
