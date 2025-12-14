// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "Constants.hpp"
#include "ExampleSmartMotorController.hpp"
#include "wpi/commands2/CommandPtr.hpp"
#include "wpi/commands2/SubsystemBase.hpp"
#include "wpi/drive/DifferentialDrive.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/math/controller/SimpleMotorFeedforward.hpp"
#include "wpi/math/trajectory/TrapezoidProfile.hpp"
#include "wpi/system/Timer.hpp"
#include "wpi/units/length.hpp"

class DriveSubsystem : public wpi::cmd::SubsystemBase {
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
  void SetDriveStates(
      wpi::math::TrapezoidProfile<wpi::units::meters>::State currentLeft,
      wpi::math::TrapezoidProfile<wpi::units::meters>::State currentRight,
      wpi::math::TrapezoidProfile<wpi::units::meters>::State nextLeft,
      wpi::math::TrapezoidProfile<wpi::units::meters>::State nextRight);

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
  wpi::units::meter_t GetLeftEncoderDistance();

  /**
   * Gets the distance of the right encoder.
   *
   * @return the average of the TWO encoder readings
   */
  wpi::units::meter_t GetRightEncoderDistance();

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
  wpi::cmd::CommandPtr ProfiledDriveDistance(wpi::units::meter_t distance);

  /**
   * Creates a command to drive forward a specified distance using a motion
   * profile without resetting the encoders.
   *
   * @param distance The distance to drive forward.
   * @return A command.
   */
  wpi::cmd::CommandPtr DynamicProfiledDriveDistance(
      wpi::units::meter_t distance);

 private:
  wpi::math::TrapezoidProfile<wpi::units::meters> m_profile{
      {DriveConstants::kMaxVelocity, DriveConstants::kMaxAcceleration}};
  wpi::Timer m_timer;
  wpi::units::meter_t m_initialLeftDistance;
  wpi::units::meter_t m_initialRightDistance;
  // Components (e.g. motor controllers and sensors) should generally be
  // declared private and exposed only through public methods.

  // The motor controllers
  ExampleSmartMotorController m_leftLeader;
  ExampleSmartMotorController m_leftFollower;
  ExampleSmartMotorController m_rightLeader;
  ExampleSmartMotorController m_rightFollower;

  // A feedforward component for the drive
  wpi::math::SimpleMotorFeedforward<wpi::units::meters> m_feedforward;

  // The robot's drive
  wpi::DifferentialDrive m_drive{
      [&](double output) { m_leftLeader.Set(output); },
      [&](double output) { m_rightLeader.Set(output); }};
};
