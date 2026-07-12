// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include "Constants.hpp"
#include "wpi/commands2/CommandPtr.hpp"
#include "wpi/commands2/SubsystemBase.hpp"
#include "wpi/drive/DifferentialDrive.hpp"
#include "wpi/hardware/imu/OnboardIMU.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"
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
  wpi::cmd::CommandPtr DriveDistanceCommand(wpi::units::meter_t distance,
                                            double velocity);

  /**
   * Returns a command that turns to robot to the specified angle using a motion
   * profile and PID controller.
   *
   * @param angle The angle to turn to
   */
  wpi::cmd::CommandPtr TurnToAngleCommand(wpi::units::degree_t angle);

 private:
  wpi::PWMSparkMax leftLeader{DriveConstants::kLeftMotor1Port};
  wpi::PWMSparkMax leftFollower{DriveConstants::kLeftMotor2Port};
  wpi::PWMSparkMax rightLeader{DriveConstants::kRightMotor1Port};
  wpi::PWMSparkMax rightFollower{DriveConstants::kRightMotor2Port};

  wpi::DifferentialDrive drive{
      [&](double output) { leftLeader.SetThrottle(output); },
      [&](double output) { rightLeader.SetThrottle(output); }};

  wpi::Encoder leftEncoder{DriveConstants::kLeftEncoderPorts[0],
                           DriveConstants::kLeftEncoderPorts[1],
                           DriveConstants::kLeftEncoderReversed};
  wpi::Encoder rightEncoder{DriveConstants::kRightEncoderPorts[0],
                            DriveConstants::kRightEncoderPorts[1],
                            DriveConstants::kRightEncoderReversed};

  wpi::OnboardIMU imu{wpi::OnboardIMU::FLAT};

  wpi::math::ProfiledPIDController<wpi::units::radians> controller{
      DriveConstants::kTurnP,
      DriveConstants::kTurnI,
      DriveConstants::kTurnD,
      {DriveConstants::kMaxTurnRate, DriveConstants::kMaxTurnAcceleration}};
  wpi::math::SimpleMotorFeedforward<wpi::units::radians> feedforward{
      DriveConstants::ks, DriveConstants::kv, DriveConstants::ka};
};
