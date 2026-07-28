// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/DriveSubsystem.hpp"

#include "wpi/system/RobotController.hpp"

using namespace DriveConstants;

DriveSubsystem::DriveSubsystem()
    : leftLeader{kLeftMotor1Port},
      leftFollower{kLeftMotor2Port},
      rightLeader{kRightMotor1Port},
      rightFollower{kRightMotor2Port},
      feedforward{ks, kv, ka} {
  wpi::util::SendableRegistry::AddChild(&drive, &leftLeader);
  wpi::util::SendableRegistry::AddChild(&drive, &rightLeader);

  // We need to invert one side of the drivetrain so that positive voltages
  // result in both sides moving forward. Depending on how your robot's
  // gearbox is constructed, you might have to invert the left side instead.
  rightLeader.SetInverted(true);

  leftFollower.Follow(leftLeader);
  rightFollower.Follow(rightLeader);

  leftLeader.SetPID(kp, 0, 0);
  rightLeader.SetPID(kp, 0, 0);
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
  leftLeader.SetSetpoint(
      ExampleSmartMotorController::PIDMode::kPosition,
      currentLeft.position.value(),
      feedforward.Calculate(currentLeft.velocity, nextLeft.velocity) /
          wpi::RobotController::GetBatteryVoltage());
  rightLeader.SetSetpoint(
      ExampleSmartMotorController::PIDMode::kPosition,
      currentRight.position.value(),
      feedforward.Calculate(currentRight.velocity, nextRight.velocity) /
          wpi::RobotController::GetBatteryVoltage());
}

void DriveSubsystem::ArcadeDrive(double fwd, double rot) {
  drive.ArcadeDrive(fwd, rot);
}

void DriveSubsystem::ResetEncoders() {
  leftLeader.ResetEncoder();
  rightLeader.ResetEncoder();
}

wpi::units::meter_t DriveSubsystem::GetLeftEncoderDistance() {
  return wpi::units::meter_t{leftLeader.GetEncoderDistance()};
}

wpi::units::meter_t DriveSubsystem::GetRightEncoderDistance() {
  return wpi::units::meter_t{rightLeader.GetEncoderDistance()};
}

void DriveSubsystem::SetMaxOutput(double maxOutput) {
  drive.SetMaxOutput(maxOutput);
}

wpi::cmd::CommandPtr DriveSubsystem::ProfiledDriveDistance(
    wpi::units::meter_t distance) {
  return StartRun(
             [&] {
               // Restart timer so profile setpoints start at the beginning
               timer.Restart();
               ResetEncoders();
             },
             [&] {
               // Current state never changes, so we need to use a timer to get
               // the setpoints we need to be at
               auto currentTime = timer.Get();
               auto currentSetpoint =
                   profile.Calculate(currentTime, {}, {distance, 0_mps});
               auto nextSetpoint =
                   profile.Calculate(currentTime + kDt, {}, {distance, 0_mps});
               SetDriveStates(currentSetpoint, currentSetpoint, nextSetpoint,
                              nextSetpoint);
             })
      .Until([&] { return profile.IsFinished(0_s); });
}

wpi::cmd::CommandPtr DriveSubsystem::DynamicProfiledDriveDistance(
    wpi::units::meter_t distance) {
  return StartRun(
             [&] {
               // Restart timer so profile setpoints start at the beginning
               timer.Restart();
               // Store distance so we know the target distance for each encoder
               initialLeftDistance = GetLeftEncoderDistance();
               initialRightDistance = GetRightEncoderDistance();
             },
             [&] {
               // Current state never changes for the duration of the command,
               // so we need to use a timer to get the setpoints we need to be
               // at
               auto currentTime = timer.Get();

               auto currentLeftSetpoint =
                   profile.Calculate(currentTime, {initialLeftDistance, 0_mps},
                                     {initialLeftDistance + distance, 0_mps});
               auto currentRightSetpoint =
                   profile.Calculate(currentTime, {initialRightDistance, 0_mps},
                                     {initialRightDistance + distance, 0_mps});

               auto nextLeftSetpoint = profile.Calculate(
                   currentTime + kDt, {initialLeftDistance, 0_mps},
                   {initialLeftDistance + distance, 0_mps});
               auto nextRightSetpoint = profile.Calculate(
                   currentTime + kDt, {initialRightDistance, 0_mps},
                   {initialRightDistance + distance, 0_mps});
               SetDriveStates(currentLeftSetpoint, currentRightSetpoint,
                              nextLeftSetpoint, nextRightSetpoint);
             })
      .Until([&] { return profile.IsFinished(0_s); });
}
