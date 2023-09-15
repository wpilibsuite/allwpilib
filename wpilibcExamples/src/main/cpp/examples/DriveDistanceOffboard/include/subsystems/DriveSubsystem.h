// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/Encoder.h>
#include <frc/controller/SimpleMotorFeedforward.h>
#include <frc/drive/DifferentialDrive.h>
#include <frc/trajectory/TrapezoidProfile.h>
#include <frc2/command/SubsystemBase.h>
#include <units/length.h>

#include "Constants.h"
#include "ExampleSmartMotorController.h"

class DriveSubsystem : public frc2::SubsystemBase {
 public:
  DriveSubsystem();

  /**
   * Will be called periodically whenever the CommandScheduler runs.
   */
  void Periodic() override;

  // Subsystem methods go here.

  /**
   * Attempts to follow the given drive states using offboard PID.
   *
   * @param left The left wheel state.
   * @param right The right wheel state.
   */
  void SetDriveStates(frc::TrapezoidProfile<units::meters>::State left,
                      frc::TrapezoidProfile<units::meters>::State right);

  /**
   * Drives the robot using arcade controls.
   *
   * @param fwd the commanded forward movement
   * @param rot the commanded rotation
   */
  void ArcadeDrive(double fwd, double rot);

  /**
   * Resets the drive encoders to currently read a position of 0.
   */
  void ResetEncoders();

  /**
   * Gets the distance of the left encoder.
   *
   * @return the average of the TWO encoder readings
   */
  units::meter_t GetLeftEncoderDistance();

  /**
   * Gets the distance of the right encoder.
   *
   * @return the average of the TWO encoder readings
   */
  units::meter_t GetRightEncoderDistance();

  /**
   * Sets the max output of the drive.  Useful for scaling the drive to drive
   * more slowly.
   *
   * @param maxOutput the maximum output to which the drive will be constrained
   */
  void SetMaxOutput(double maxOutput);

 private:
  // Components (e.g. motor controllers and sensors) should generally be
  // declared private and exposed only through public methods.

  // The motor controllers
  ExampleSmartMotorController m_leftLeader;
  ExampleSmartMotorController m_leftFollower;
  ExampleSmartMotorController m_rightLeader;
  ExampleSmartMotorController m_rightFollower;

  // A feedforward component for the drive
  frc::SimpleMotorFeedforward<units::meters> m_feedforward;

  // The robot's drive
  frc::DifferentialDrive m_drive{m_leftLeader, m_rightLeader};
};
