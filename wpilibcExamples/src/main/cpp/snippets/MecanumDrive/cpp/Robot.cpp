// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/drive/MecanumDrive.hpp"
#include "wpi/drivers/motor/PWMSparkMax.hpp"
#include "wpi/driverstation/Joystick.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/hardware/imu/OnboardIMU.hpp"

/**
 * This is a sample program that uses mecanum drive with a gyro sensor to
 * maintain rotation vectors in relation to the starting orientation of the
 * robot (field-oriented controls).
 *
 * Finally, short code snippets show how to use cartesian and polar drive
 * methods for wpilib-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/motors/wpi-drive-classes.html
 */
class Robot : public wpi::TimedRobot {
 public:
  Robot() {
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
     * movement, and Z axis for rotation, field oriented.
     */
    robotDrive.DriveCartesian(-joystick.GetY(), -joystick.GetX(),
                              -joystick.GetZ(), imu.GetRotation2d());
    // Drive using the X, Y, and Z axes of the joystick.
    robotDrive.DriveCartesian(-joystick.GetY(), -joystick.GetX(),
                              -joystick.GetZ());
    // Drive at 45 degrees relative to the robot, at the speed given by the Y
    // axis of the joystick, with no rotation.
    robotDrive.DrivePolar(-joystick.GetY(), 45_deg, 0);
  }

 private:
  static constexpr int FRONT_LEFT_MOTOR_PORT = 0;
  static constexpr int REAR_LEFT_MOTOR_PORT = 1;
  static constexpr int FRONT_RIGHT_MOTOR_PORT = 2;
  static constexpr int REAR_RIGHT_MOTOR_PORT = 3;
  static constexpr wpi::OnboardIMU::MountOrientation IMU_MOUNT_ORIENTATION =
      wpi::OnboardIMU::FLAT;
  static constexpr int JOYSTICK_PORT = 0;

  wpi::PWMSparkMax frontLeft{FRONT_LEFT_MOTOR_PORT};
  wpi::PWMSparkMax rearLeft{REAR_LEFT_MOTOR_PORT};
  wpi::PWMSparkMax frontRight{FRONT_RIGHT_MOTOR_PORT};
  wpi::PWMSparkMax rearRight{REAR_RIGHT_MOTOR_PORT};
  wpi::MecanumDrive robotDrive{
      [&](double output) { frontLeft.SetThrottle(output); },
      [&](double output) { rearLeft.SetThrottle(output); },
      [&](double output) { frontRight.SetThrottle(output); },
      [&](double output) { rearRight.SetThrottle(output); }};

  wpi::OnboardIMU imu{IMU_MOUNT_ORIENTATION};
  wpi::Joystick joystick{JOYSTICK_PORT};
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
