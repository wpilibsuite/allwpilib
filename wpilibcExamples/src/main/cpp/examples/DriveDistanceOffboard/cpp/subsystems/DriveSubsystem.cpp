// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/DriveSubsystem.hpp"

#include "wpi/system/RobotController.hpp"

using namespace DriveConstants;

DriveSubsystem::DriveSubsystem()
    : leftLeader{LEFT_MOTOR1_PORT},
      leftFollower{LEFT_MOTOR2_PORT},
      rightLeader{RIGHT_MOTOR1_PORT},
      rightFollower{RIGHT_MOTOR2_PORT},
      feedforward{ks, kv, ka} {
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
    wpi::math::TrapezoidProfile<wpi::units::meters_>::State currentLeft,
    wpi::math::TrapezoidProfile<wpi::units::meters_>::State currentRight,
    wpi::math::TrapezoidProfile<wpi::units::meters_>::State nextLeft,
    wpi::math::TrapezoidProfile<wpi::units::meters_>::State nextRight) {
  // Feedforward is divided by battery voltage to normalize it to [-1, 1]
  leftLeader.SetSetpoint(
      ExampleSmartMotorController::PIDMode::POSITION,
      currentLeft.position.value(),
      feedforward.Calculate(currentLeft.velocity, nextLeft.velocity) /
          wpi::RobotController::GetBatteryVoltage());
  rightLeader.SetSetpoint(
      ExampleSmartMotorController::PIDMode::POSITION,
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

wpi::units::meters<> DriveSubsystem::GetLeftEncoderDistance() {
  return wpi::units::meters<>{leftLeader.GetEncoderDistance()};
}

wpi::units::meters<> DriveSubsystem::GetRightEncoderDistance() {
  return wpi::units::meters<>{rightLeader.GetEncoderDistance()};
}

void DriveSubsystem::SetMaxOutput(double maxOutput) {
  drive.SetMaxOutput(maxOutput);
}

wpi::cmd::CommandPtr DriveSubsystem::ProfiledDriveDistance(
    wpi::units::meters<> distance) {
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
                   profile.Calculate(currentTime + DT, {}, {distance, 0_mps});
               SetDriveStates(currentSetpoint, currentSetpoint, nextSetpoint,
                              nextSetpoint);
             })
      .Until([&] { return profile.IsFinished(0_s); });
}

wpi::cmd::CommandPtr DriveSubsystem::DynamicProfiledDriveDistance(
    wpi::units::meters<> distance) {
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
                   currentTime + DT, {initialLeftDistance, 0_mps},
                   {initialLeftDistance + distance, 0_mps});
               auto nextRightSetpoint = profile.Calculate(
                   currentTime + DT, {initialRightDistance, 0_mps},
                   {initialRightDistance + distance, 0_mps});
               SetDriveStates(currentLeftSetpoint, currentRightSetpoint,
                              nextLeftSetpoint, nextRightSetpoint);
             })
      .Until([&] { return profile.IsFinished(0_s); });
}
