// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

// the WPILib BSD license file in the root directory of this project.

#include "subsystems/DriveSubsystem.h"

#include <frc/Timer.h>

using namespace DriveConstants;

DriveSubsystem::DriveSubsystem()
    : m_leftLeader{kLeftMotor1Port},
      m_leftFollower{kLeftMotor2Port},
      m_rightLeader{kRightMotor1Port},
      m_rightFollower{kRightMotor2Port},
      m_feedforward{ks, kv, ka} {
  wpi::SendableRegistry::AddChild(&m_drive, &m_leftLeader);
  wpi::SendableRegistry::AddChild(&m_drive, &m_rightLeader);

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
    frc::TrapezoidProfile<units::meters>::State left,
    frc::TrapezoidProfile<units::meters>::State right) {
  m_leftLeader.SetSetpoint(ExampleSmartMotorController::PIDMode::kPosition,
                           left.position.value(),
                           m_feedforward.Calculate(left.velocity) / 12_V);
  m_rightLeader.SetSetpoint(ExampleSmartMotorController::PIDMode::kPosition,
                            right.position.value(),
                            m_feedforward.Calculate(right.velocity) / 12_V);
}

void DriveSubsystem::ArcadeDrive(double fwd, double rot) {
  m_drive.ArcadeDrive(fwd, rot);
}

void DriveSubsystem::ResetEncoders() {
  m_leftLeader.ResetEncoder();
  m_rightLeader.ResetEncoder();
}

units::meter_t DriveSubsystem::GetLeftEncoderDistance() {
  return units::meter_t{m_leftLeader.GetEncoderDistance()};
}

units::meter_t DriveSubsystem::GetRightEncoderDistance() {
  return units::meter_t{m_rightLeader.GetEncoderDistance()};
}

void DriveSubsystem::SetMaxOutput(double maxOutput) {
  m_drive.SetMaxOutput(maxOutput);
}

frc2::CommandPtr DriveSubsystem::ProfiledDriveDistance(
    units::meter_t distance) {
  auto profile =
      frc::TrapezoidProfile<units::meters>{{kMaxSpeed, kMaxAcceleration}};
  frc::Timer timer{};
  return StartRun(
             [&] {
               timer.Restart();
               ResetEncoders();
             },
             [&] {
               auto setpoint = profile.Calculate(
                   timer.Get(), {}, {distance, units::meters_per_second_t{0}});
               SetDriveStates(setpoint, setpoint);
             })
      .Until([&] { return profile.IsFinished(units::second_t{0}); });
}

frc2::CommandPtr DriveSubsystem::DynamicProfiledDriveDistance(
    units::meter_t distance) {
  auto profile =
      frc::TrapezoidProfile<units::meters>{{kMaxSpeed, kMaxAcceleration}};
  return StartRun(
             [&] {
               m_initialLeftDistance = GetLeftEncoderDistance();
               m_initialRightDistance = GetRightEncoderDistance();
             },
             [&] {
               auto leftSetpoint = profile.Calculate(
                   kDt,
                   {GetLeftEncoderDistance(),
                    units::meters_per_second_t{m_leftLeader.GetEncoderRate()}},
                   {m_initialLeftDistance + distance,
                    units::meters_per_second_t{0}});
               auto rightSetpoint = profile.Calculate(
                   kDt,
                   {GetLeftEncoderDistance(),
                    units::meters_per_second_t{m_rightLeader.GetEncoderRate()}},
                   {m_initialRightDistance + distance,
                    units::meters_per_second_t{0}});
               SetDriveStates(leftSetpoint, rightSetpoint);
             })
      .Until([&] { return profile.IsFinished(units::second_t{0}); });
}
