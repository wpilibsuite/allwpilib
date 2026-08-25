// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include "Constants.hpp"
#include "wpi/commands2/CommandPtr.hpp"
#include "wpi/commands2/SubsystemBase.hpp"
#include "wpi/drive/DifferentialDrive.hpp"
#include "wpi/drivers/motor/PWMSparkMax.hpp"
#include "wpi/hardware/imu/OnboardIMU.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/math/controller/ProfiledPIDController.hpp"
#include "wpi/math/controller/SimpleMotorFeedforward.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"

class Drive : public wpi::cmd::SubsystemBase {
 public:
  Drive();
  /**
   * Returns a command that drives the robot with arcade controls.
   *
   * @param fwd the commanded forward movement
   * @param rot the commanded rotation
   */
  wpi::cmd::CommandPtr ArcadeDriveCommand(std::function<double()> fwd,
                                          std::function<double()> rot);

  /**
   * Returns a command that drives the robot forward a specified distance at a
   * specified velocity.
   *
   * @param distance The distance to drive forward in meters
   * @param velocity The fraction of max velocity at which to drive
   */
  wpi::cmd::CommandPtr DriveDistanceCommand(wpi::units::meters<> distance,
                                            double velocity);

  /**
   * Returns a command that turns to robot to the specified angle using a motion
   * profile and PID controller.
   *
   * @param angle The angle to turn to
   */
  wpi::cmd::CommandPtr TurnToAngleCommand(wpi::units::degrees<> angle);

 private:
  wpi::PWMSparkMax leftLeader{DriveConstants::LEFT_MOTOR1_PORT};
  wpi::PWMSparkMax leftFollower{DriveConstants::LEFT_MOTOR2_PORT};
  wpi::PWMSparkMax rightLeader{DriveConstants::RIGHT_MOTOR1_PORT};
  wpi::PWMSparkMax rightFollower{DriveConstants::RIGHT_MOTOR2_PORT};

  wpi::DifferentialDrive drive{
      [&](double output) { leftLeader.SetThrottle(output); },
      [&](double output) { rightLeader.SetThrottle(output); }};

  wpi::Encoder leftEncoder{DriveConstants::LEFT_ENCODER_PORTS[0],
                           DriveConstants::LEFT_ENCODER_PORTS[1],
                           DriveConstants::LEFT_ENCODER_REVERSED};
  wpi::Encoder rightEncoder{DriveConstants::RIGHT_ENCODER_PORTS[0],
                            DriveConstants::RIGHT_ENCODER_PORTS[1],
                            DriveConstants::RIGHT_ENCODER_REVERSED};

  wpi::OnboardIMU imu{wpi::OnboardIMU::FLAT};

  wpi::math::ProfiledPIDController<wpi::units::radians_> controller{
      DriveConstants::TURN_P,
      DriveConstants::TURN_I,
      DriveConstants::TURN_D,
      {DriveConstants::MAX_TURN_RATE, DriveConstants::MAX_TURN_ACCELERATION}};
  wpi::math::SimpleMotorFeedforward<wpi::units::radians_> feedforward{
      DriveConstants::ks, DriveConstants::kv, DriveConstants::ka};
};
