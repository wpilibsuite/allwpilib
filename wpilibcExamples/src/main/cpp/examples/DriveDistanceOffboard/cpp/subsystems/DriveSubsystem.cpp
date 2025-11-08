// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/DriveSubsystem.hpp"

#include <wpi/system/RobotController.hpp>

using namespace DriveConstants;

DriveSubsystem::DriveSubsystem()
    : m_leftLeader{kLeftMotor1Port},
      m_leftFollower{kLeftMotor2Port},
      m_rightLeader{kRightMotor1Port},
      m_rightFollower{kRightMotor2Port},
      m_feedforward{ks, kv, ka} {
  wpi::util::SendableRegistry::AddChild(&m_drive, &m_leftLeader);
  wpi::util::SendableRegistry::AddChild(&m_drive, &m_rightLeader);

  // We need to invert one side of the drivetrain so that positive voltages
  // result in both sides moving forward. Depending on how your robot's
  // gearbox is constructed, you might have to invert the left side instead.
  m_rightLeader.SetInverted(true);

  m_leftFollower.Follow(m_leftLeader);
  m_rightFollower.Follow(m_rightLeader);

  m_leftLeader.SetPID(kp, 0, 0);
  m_rightLeader.SetPID(kp, 0, 0);
}

void DriveSubsystem::Periodic() {
  // Implementation of subsystem periodic method goes here.
}

void DriveSubsystem::SetDriveStates(
    wpi::math::TrapezoidProfile<wpi::units::meters>::State currentLeft,
    wpi::math::TrapezoidProfile<wpi::units::meters>::State currentRight,
    wpi::math::TrapezoidProfile<wpi::units::meters>::State nextLeft,
    wpi::math::TrapezoidProfile<wpi::units::meters>::State nextRight) {
  // Feedforward is divided by battery voltage to normalize it to [-1, 1]
  m_leftLeader.SetSetpoint(
      ExampleSmartMotorController::PIDMode::kPosition,
      currentLeft.position.value(),
      m_feedforward.Calculate(currentLeft.velocity, nextLeft.velocity) /
          wpi::RobotController::GetBatteryVoltage());
  m_rightLeader.SetSetpoint(
      ExampleSmartMotorController::PIDMode::kPosition,
      currentRight.position.value(),
      m_feedforward.Calculate(currentRight.velocity, nextRight.velocity) /
          wpi::RobotController::GetBatteryVoltage());
}

void DriveSubsystem::ArcadeDrive(double fwd, double rot) {
  m_drive.ArcadeDrive(fwd, rot);
}

void DriveSubsystem::ResetEncoders() {
  m_leftLeader.ResetEncoder();
  m_rightLeader.ResetEncoder();
}

wpi::units::meter_t DriveSubsystem::GetLeftEncoderDistance() {
  return wpi::units::meter_t{m_leftLeader.GetEncoderDistance()};
}

wpi::units::meter_t DriveSubsystem::GetRightEncoderDistance() {
  return wpi::units::meter_t{m_rightLeader.GetEncoderDistance()};
}

void DriveSubsystem::SetMaxOutput(double maxOutput) {
  m_drive.SetMaxOutput(maxOutput);
}

wpi::cmd::CommandPtr DriveSubsystem::ProfiledDriveDistance(
    wpi::units::meter_t distance) {
  return StartRun(
             [&] {
               // Restart timer so profile setpoints start at the beginning
               m_timer.Restart();
               ResetEncoders();
             },
             [&] {
               // Current state never changes, so we need to use a timer to get
               // the setpoints we need to be at
               auto currentTime = m_timer.Get();
               auto currentSetpoint =
                   m_profile.Calculate(currentTime, {}, {distance, 0_mps});
               auto nextSetpoint = m_profile.Calculate(currentTime + kDt, {},
                                                       {distance, 0_mps});
               SetDriveStates(currentSetpoint, currentSetpoint, nextSetpoint,
                              nextSetpoint);
             })
      .Until([&] { return m_profile.IsFinished(0_s); });
}

wpi::cmd::CommandPtr DriveSubsystem::DynamicProfiledDriveDistance(
    wpi::units::meter_t distance) {
  return StartRun(
             [&] {
               // Restart timer so profile setpoints start at the beginning
               m_timer.Restart();
               // Store distance so we know the target distance for each encoder
               m_initialLeftDistance = GetLeftEncoderDistance();
               m_initialRightDistance = GetRightEncoderDistance();
             },
             [&] {
               // Current state never changes for the duration of the command,
               // so we need to use a timer to get the setpoints we need to be
               // at
               auto currentTime = m_timer.Get();

               auto currentLeftSetpoint = m_profile.Calculate(
                   currentTime, {m_initialLeftDistance, 0_mps},
                   {m_initialLeftDistance + distance, 0_mps});
               auto currentRightSetpoint = m_profile.Calculate(
                   currentTime, {m_initialRightDistance, 0_mps},
                   {m_initialRightDistance + distance, 0_mps});

               auto nextLeftSetpoint = m_profile.Calculate(
                   currentTime + kDt, {m_initialLeftDistance, 0_mps},
                   {m_initialLeftDistance + distance, 0_mps});
               auto nextRightSetpoint = m_profile.Calculate(
                   currentTime + kDt, {m_initialRightDistance, 0_mps},
                   {m_initialRightDistance + distance, 0_mps});
               SetDriveStates(currentLeftSetpoint, currentRightSetpoint,
                              nextLeftSetpoint, nextRightSetpoint);
             })
      .Until([&] { return m_profile.IsFinished(0_s); });
}
