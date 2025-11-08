// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <wpi/commands2/CommandPtr.hpp>
#include <wpi/commands2/SubsystemBase.hpp>
#include <wpi/drive/DifferentialDrive.hpp>
#include <wpi/hardware/imu/OnboardIMU.hpp>
#include <wpi/hardware/motor/PWMSparkMax.hpp>
#include <wpi/hardware/rotation/Encoder.hpp>
#include <wpi/math/controller/ProfiledPIDController.hpp>
#include <wpi/math/controller/SimpleMotorFeedforward.hpp>
#include <wpi/units/angle.hpp>
#include <wpi/units/length.hpp>

#include "Constants.hpp"

class Drive : public frc2::SubsystemBase {
 public:
  Drive();
  /**
   * Returns a command that drives the robot with arcade controls.
   *
   * @param fwd the commanded forward movement
   * @param rot the commanded rotation
   */
  frc2::CommandPtr ArcadeDriveCommand(std::function<double()> fwd,
                                      std::function<double()> rot);

  /**
   * Returns a command that drives the robot forward a specified distance at a
   * specified speed.
   *
   * @param distance The distance to drive forward in meters
   * @param speed The fraction of max speed at which to drive
   */
  frc2::CommandPtr DriveDistanceCommand(units::meter_t distance, double speed);

  /**
   * Returns a command that turns to robot to the specified angle using a motion
   * profile and PID controller.
   *
   * @param angle The angle to turn to
   */
  frc2::CommandPtr TurnToAngleCommand(units::degree_t angle);

 private:
  frc::PWMSparkMax m_leftLeader{DriveConstants::kLeftMotor1Port};
  frc::PWMSparkMax m_leftFollower{DriveConstants::kLeftMotor2Port};
  frc::PWMSparkMax m_rightLeader{DriveConstants::kRightMotor1Port};
  frc::PWMSparkMax m_rightFollower{DriveConstants::kRightMotor2Port};

  frc::DifferentialDrive m_drive{
      [&](double output) { m_leftLeader.Set(output); },
      [&](double output) { m_rightLeader.Set(output); }};

  frc::Encoder m_leftEncoder{DriveConstants::kLeftEncoderPorts[0],
                             DriveConstants::kLeftEncoderPorts[1],
                             DriveConstants::kLeftEncoderReversed};
  frc::Encoder m_rightEncoder{DriveConstants::kRightEncoderPorts[0],
                              DriveConstants::kRightEncoderPorts[1],
                              DriveConstants::kRightEncoderReversed};

  frc::OnboardIMU m_imu{frc::OnboardIMU::kFlat};

  frc::ProfiledPIDController<units::radians> m_controller{
      DriveConstants::kTurnP,
      DriveConstants::kTurnI,
      DriveConstants::kTurnD,
      {DriveConstants::kMaxTurnRate, DriveConstants::kMaxTurnAcceleration}};
  frc::SimpleMotorFeedforward<units::radians> m_feedforward{
      DriveConstants::ks, DriveConstants::kv, DriveConstants::ka};
};
