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
   * specified speed.
   *
   * @param distance The distance to drive forward in meters
   * @param speed The fraction of max speed at which to drive
   */
  wpi::cmd::CommandPtr DriveDistanceCommand(wpi::units::meter_t distance,
                                            double speed);

  /**
   * Returns a command that turns to robot to the specified angle using a motion
   * profile and PID controller.
   *
   * @param angle The angle to turn to
   */
  wpi::cmd::CommandPtr TurnToAngleCommand(wpi::units::degree_t angle);

 private:
  wpi::PWMSparkMax m_leftLeader{DriveConstants::kLeftMotor1Port};
  wpi::PWMSparkMax m_leftFollower{DriveConstants::kLeftMotor2Port};
  wpi::PWMSparkMax m_rightLeader{DriveConstants::kRightMotor1Port};
  wpi::PWMSparkMax m_rightFollower{DriveConstants::kRightMotor2Port};

  wpi::DifferentialDrive m_drive{
      [&](double output) { m_leftLeader.Set(output); },
      [&](double output) { m_rightLeader.Set(output); }};

  wpi::Encoder m_leftEncoder{DriveConstants::kLeftEncoderPorts[0],
                             DriveConstants::kLeftEncoderPorts[1],
                             DriveConstants::kLeftEncoderReversed};
  wpi::Encoder m_rightEncoder{DriveConstants::kRightEncoderPorts[0],
                              DriveConstants::kRightEncoderPorts[1],
                              DriveConstants::kRightEncoderReversed};

  wpi::OnboardIMU m_imu{wpi::OnboardIMU::kFlat};

  wpi::math::ProfiledPIDController<wpi::units::radians> m_controller{
      DriveConstants::kTurnP,
      DriveConstants::kTurnI,
      DriveConstants::kTurnD,
      {DriveConstants::kMaxTurnRate, DriveConstants::kMaxTurnAcceleration}};
  wpi::math::SimpleMotorFeedforward<wpi::units::radians> m_feedforward{
      DriveConstants::ks, DriveConstants::kv, DriveConstants::ka};
};
