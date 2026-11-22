// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>

#include "wpi/drive/DifferentialDrive.hpp"
#include "wpi/drivers/motor/PWMSparkMax.hpp"
#include "wpi/driverstation/Joystick.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/hardware/imu/OnboardIMU.hpp"

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
  }

  /**
   * The motor velocity is set from the joystick while the DifferentialDrive
   * turning value is assigned from the error between the setpoint and the gyro
   * angle.
   */
  void TeleopPeriodic() override {
    double turningValue =
        (ANGLE_SETPOINT - imu.GetRotation2d().Degrees().value()) * kP;
    drive.ArcadeDrive(-joystick.GetY(), -turningValue);
  }

 private:
  static constexpr double ANGLE_SETPOINT = 0.0;
  static constexpr double kP = 0.005;  // Proportional turning constant

  static constexpr int LEFT_MOTOR_PORT = 0;
  static constexpr int RIGHT_MOTOR_PORT = 1;
  static constexpr wpi::OnboardIMU::MountOrientation IMU_MOUNT_ORIENTATION =
      wpi::OnboardIMU::FLAT;
  static constexpr int JOYSTICK_PORT = 0;

  wpi::PWMSparkMax left{LEFT_MOTOR_PORT};
  wpi::PWMSparkMax right{RIGHT_MOTOR_PORT};
  wpi::DifferentialDrive drive{
      [&](double output) { left.SetThrottle(output); },
      [&](double output) { right.SetThrottle(output); }};

  wpi::OnboardIMU imu{IMU_MOUNT_ORIENTATION};
  wpi::Joystick joystick{JOYSTICK_PORT};
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
