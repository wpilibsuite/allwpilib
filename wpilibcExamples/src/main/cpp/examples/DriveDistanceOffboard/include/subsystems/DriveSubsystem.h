// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/Encoder.h>
#include <frc/Timer.h>
#include <frc/controller/SimpleMotorFeedforward.h>
#include <frc/drive/DifferentialDrive.h>
#include <frc/trajectory/TrapezoidProfile.h>
#include <frc2/command/CommandPtr.h>
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
   * @param currentLeft The current left wheel state.
   * @param currentRight The current right wheel state.
   * @param nextLeft The next left wheel state.
   * @param nextRight The next right wheel state.
   */
  void SetDriveStates(frc::TrapezoidProfile<units::meters>::State currentLeft,
                      frc::TrapezoidProfile<units::meters>::State currentRight,
                      frc::TrapezoidProfile<units::meters>::State nextLeft,
                      frc::TrapezoidProfile<units::meters>::State nextRight);

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

  /**
   * Creates a command to drive forward a specified distance using a motion
   * profile.
   *
   * @param distance The distance to drive forward.
   * @return A command.
   */
  [[nodiscard]]
  frc2::CommandPtr ProfiledDriveDistance(units::meter_t distance);

  /**
   * Creates a command to drive forward a specified distance using a motion
   * profile without resetting the encoders.
   *
   * @param distance The distance to drive forward.
   * @return A command.
   */
  [[nodiscard]]
  frc2::CommandPtr DynamicProfiledDriveDistance(units::meter_t distance);

 private:
  frc::TrapezoidProfile<units::meters> m_profile{
      {DriveConstants::kMaxSpeed, DriveConstants::kMaxAcceleration}};
  frc::Timer m_timer;
  units::meter_t m_initialLeftDistance;
  units::meter_t m_initialRightDistance;
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
  frc::DifferentialDrive m_drive{
      [&](double output) { m_leftLeader.Set(output); },
      [&](double output) { m_rightLeader.Set(output); }};
};
