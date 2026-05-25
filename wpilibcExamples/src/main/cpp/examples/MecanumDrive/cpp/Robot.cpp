// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/drive/MecanumDrive.hpp"
#include "wpi/driverstation/Joystick.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/hardware/imu/OnboardIMU.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"

/**
 * This is a sample program that uses mecanum drive with a gyro sensor to
 * maintain rotation vectors in relation to the starting orientation of the
 * robot (field-oriented controls).
 */
class Robot : public wpi::TimedRobot {
 public:
  Robot() {
    wpi::util::SendableRegistry::AddChild(&robotDrive, &frontLeft);
    wpi::util::SendableRegistry::AddChild(&robotDrive, &rearLeft);
    wpi::util::SendableRegistry::AddChild(&robotDrive, &frontRight);
    wpi::util::SendableRegistry::AddChild(&robotDrive, &rearRight);

    // Invert the right side motors. You may need to change or remove this to
    // match your robot.
    frontRight.SetInverted(true);
    rearRight.SetInverted(true);
  }

  /**
   * Mecanum drive is used with the gyro angle as an input.
   */
  void TeleopPeriodic() override {
    /* Use the joystick Y axis for forward movement, X axis for lateral
     * movement, and Z axis for rotation.
     */
    robotDrive.DriveCartesian(-joystick.GetY(), -joystick.GetX(),
                              -joystick.GetZ(), imu.GetRotation2d());
  }

 private:
  static constexpr int kFrontLeftMotorPort = 0;
  static constexpr int kRearLeftMotorPort = 1;
  static constexpr int kFrontRightMotorPort = 2;
  static constexpr int kRearRightMotorPort = 3;
  static constexpr wpi::OnboardIMU::MountOrientation kIMUMountOrientation =
      wpi::OnboardIMU::FLAT;
  static constexpr int kJoystickPort = 0;

  wpi::PWMSparkMax frontLeft{kFrontLeftMotorPort};
  wpi::PWMSparkMax rearLeft{kRearLeftMotorPort};
  wpi::PWMSparkMax frontRight{kFrontRightMotorPort};
  wpi::PWMSparkMax rearRight{kRearRightMotorPort};
  wpi::MecanumDrive robotDrive{
      [&](double output) { frontLeft.SetThrottle(output); },
      [&](double output) { rearLeft.SetThrottle(output); },
      [&](double output) { frontRight.SetThrottle(output); },
      [&](double output) { rearRight.SetThrottle(output); }};

  wpi::OnboardIMU imu{kIMUMountOrientation};
  wpi::Joystick joystick{kJoystickPort};
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
