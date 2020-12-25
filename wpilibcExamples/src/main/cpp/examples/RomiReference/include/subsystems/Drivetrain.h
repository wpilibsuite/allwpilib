/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/ADXRS450_Gyro.h>
#include <frc/Encoder.h>
#include <frc/PWMVictorSPX.h>
#include <frc/SpeedControllerGroup.h>
#include <frc/drive/DifferentialDrive.h>
#include <frc/geometry/Pose2d.h>
#include <frc/kinematics/DifferentialDriveOdometry.h>
#include <frc2/command/SubsystemBase.h>
#include <units/voltage.h>

#include "Constants.h"

class Drivetrain : public frc2::SubsystemBase {
 public:
  Drivetrain();

  /**
   * Will be called periodically whenever the CommandScheduler runs.
   */
  void Periodic() override;

  /**
   * Drives the robot using arcade controls.
   *
   * @param xaxisSpeed the commanded forward movement
   * @param zaxisRotate the commanded rotation
   */
  void ArcadeDrive(double xaxisSpeed, double zaxisRotate);

  /**
   * Resets the drive encoders to currently read a position of 0.
   */
  void ResetEncoders();

  /**
   * Gets the left drive encoder count.
   *
   * @return the left drive encoder count
   */
  int GetLeftEncoderCount();

  /**
  * Gets the right drive encoder count.
  *
  * @return the right drive encoder count
  */
  int GetRightEncoderCount();

  /**
   * Gets the left distance driven.
   *
   * @return the left-side distance driven 
   */
  units::length_t GetLeftDistance();

  /**
  * Gets the right distance driven.
  *
  * @return the right-side distance driven 
  */
  units::length_t GetRightDistance();

 private:
  frc::PWMVictorSPX m_left;
  frc::PWMVictorSPX m_right;

  // Set up the differential drive controller
  frc::DifferentialDrive m_drive{m_left, m_right};

  // The left-side drive encoder
  frc::Encoder m_leftEncoder;

  // The right-side drive encoder
  frc::Encoder m_rightEncoder;
};
