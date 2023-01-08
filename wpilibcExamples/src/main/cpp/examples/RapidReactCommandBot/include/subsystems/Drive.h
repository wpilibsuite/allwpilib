// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <frc/Encoder.h>
#include <frc/drive/DifferentialDrive.h>
#include <frc/motorcontrol/MotorControllerGroup.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc2/command/CommandPtr.h>
#include <frc2/command/SubsystemBase.h>
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
  [[nodiscard]] frc2::CommandPtr ArcadeDriveCommand(
      std::function<double()> fwd, std::function<double()> rot);

  /**
   * Returns a command that drives the robot forward a specified distance at a
   * specified speed.
   *
   * @param distance The distance to drive forward in meters
   * @param speed The fraction of max speed at which to drive
   */
  [[nodiscard]] frc2::CommandPtr DriveDistanceCommand(units::meter_t distance,
                                                      double speed);

 private:
  frc::PWMSparkMax m_leftLeader{DriveConstants::kLeftMotor1Port};
  frc::PWMSparkMax m_leftFollower{DriveConstants::kLeftMotor2Port};
  frc::PWMSparkMax m_rightLeader{DriveConstants::kRightMotor1Port};
  frc::PWMSparkMax m_rightFollower{DriveConstants::kRightMotor2Port};

  frc::MotorControllerGroup m_leftMotors{m_leftLeader, m_leftFollower};
  frc::MotorControllerGroup m_rightMotors{m_rightLeader, m_rightFollower};

  frc::DifferentialDrive m_drive{m_leftMotors, m_rightMotors};

  frc::Encoder m_leftEncoder{DriveConstants::kLeftEncoderPorts[0],
                             DriveConstants::kLeftEncoderPorts[1],
                             DriveConstants::kLeftEncoderReversed};
  frc::Encoder m_rightEncoder{DriveConstants::kRightEncoderPorts[0],
                              DriveConstants::kRightEncoderPorts[1],
                              DriveConstants::kRightEncoderReversed};
};
