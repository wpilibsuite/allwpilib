// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <frc/AnalogGyro.h>
#include <frc/Encoder.h>
#include <frc/controller/ProfiledPIDController.h>
#include <frc/controller/SimpleMotorFeedforward.h>
#include <frc/drive/DifferentialDrive.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc2/command/CommandPtr.h>
#include <frc2/command/SubsystemBase.h>
#include <units/angle.h>
#include <units/length.h>

#include "Constants.h"

class Drive : public frc2::SubsystemBase {
 public:
  Drive();
  /**
   * Returns a command that drives the robot with arcade controls.
   *
   * @param fwd the commanded forward movement
   * @param rot the commanded rotation
   */
  [[nodiscard]]
  frc2::CommandPtr ArcadeDriveCommand(std::function<double()> fwd,
                                      std::function<double()> rot);

  /**
   * Returns a command that drives the robot forward a specified distance at a
   * specified speed.
   *
   * @param distance The distance to drive forward in meters
   * @param speed The fraction of max speed at which to drive
   */
  [[nodiscard]]
  frc2::CommandPtr DriveDistanceCommand(units::meter_t distance, double speed);

  /**
   * Returns a command that turns to robot to the specified angle using a motion
   * profile and PID controller.
   *
   * @param angle The angle to turn to
   */
  [[nodiscard]]
  frc2::CommandPtr TurnToAngleCommand(units::degree_t angle);

 private:
  frc::PWMSparkMax m_leftLeader{DriveConstants::LEFT_MOTOR_1_PORT};
  frc::PWMSparkMax m_leftFollower{DriveConstants::LEFT_MOTOR_2_PORT};
  frc::PWMSparkMax m_rightLeader{DriveConstants::RIGHT_MOTOR_1_PORT};
  frc::PWMSparkMax m_rightFollower{DriveConstants::RIGHT_MOTOR_2_PORT};

  frc::DifferentialDrive m_drive{
      [&](double output) { m_leftLeader.Set(output); },
      [&](double output) { m_rightLeader.Set(output); }};

  frc::Encoder m_leftEncoder{DriveConstants::LEFT_ENCODER_PORTS[0],
                             DriveConstants::LEFT_ENCODER_PORTS[1],
                             DriveConstants::LEFT_ENCODER_REVERSED};
  frc::Encoder m_rightEncoder{DriveConstants::RIGHT_ENCODER_PORTS[0],
                              DriveConstants::RIGHT_ENCODER_PORTS[1],
                              DriveConstants::RIGHT_ENCODER_REVERSED};

  frc::AnalogGyro m_gyro{0};

  frc::ProfiledPIDController<units::radians> m_controller{
      DriveConstants::TURN_P,
      DriveConstants::TURN_I,
      DriveConstants::TURN_D,
      {DriveConstants::MAX_TURN_RATE, DriveConstants::MAX_TURN_ACCELERATION}};
  frc::SimpleMotorFeedforward<units::radians> m_feedforward{
      DriveConstants::ks, DriveConstants::kv, DriveConstants::ka};
};
