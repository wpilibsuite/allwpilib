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
    const wpi::math::TrapezoidProfileSample<wpi::units::meters>& currentLeft,
    const wpi::math::TrapezoidProfileSample<wpi::units::meters>& currentRight,
    const wpi::math::TrapezoidProfileSample<wpi::units::meters>& nextLeft,
    const wpi::math::TrapezoidProfileSample<wpi::units::meters>& nextRight) {
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
             [this, distance] {
               // Restart timer so profile setpoints start at the beginning
               timer.Restart();
               ResetEncoders();
               // Both encoders start at zero, so they can share a profile
               leftProfile =
                   wpi::math::TrapezoidProfile<wpi::units::meters>::Generate(
                       constraints, {}, {distance, 0_mps});
             },
             [this] {
               // Current state never changes, so we need to use a timer to get
               // the setpoints we need to be at
               auto currentTime = timer.Get();
               auto currentSetpoint = leftProfile.SampleAt(currentTime);
               auto nextSetpoint = leftProfile.SampleAt(currentTime + DT);
               SetDriveStates(currentSetpoint, currentSetpoint, nextSetpoint,
                              nextSetpoint);
             })
      .Until([this] { return timer.Get() >= leftProfile.Duration(); });
}

wpi::cmd::CommandPtr DriveSubsystem::DynamicProfiledDriveDistance(
    wpi::units::meter_t distance) {
  return StartRun(
             [this, distance] {
               // Restart timer so profile setpoints start at the beginning
               timer.Restart();
               // Store distance so we know the target distance for each encoder
               initialLeftDistance = GetLeftEncoderDistance();
               initialRightDistance = GetRightEncoderDistance();
               leftProfile =
                   wpi::math::TrapezoidProfile<wpi::units::meters>::Generate(
                       constraints, {initialLeftDistance, 0_mps},
                       {initialLeftDistance + distance, 0_mps});
               rightProfile =
                   wpi::math::TrapezoidProfile<wpi::units::meters>::Generate(
                       constraints, {initialRightDistance, 0_mps},
                       {initialRightDistance + distance, 0_mps});
             },
             [this] {
               // Current state never changes for the duration of the command,
               // so we need to use a timer to get the setpoints we need to be
               // at
               auto currentTime = timer.Get();

               auto currentLeftSetpoint = leftProfile.SampleAt(currentTime);
               auto currentRightSetpoint = rightProfile.SampleAt(currentTime);

               auto nextLeftSetpoint = leftProfile.SampleAt(currentTime + DT);
               auto nextRightSetpoint = rightProfile.SampleAt(currentTime + DT);
               SetDriveStates(currentLeftSetpoint, currentRightSetpoint,
                              nextLeftSetpoint, nextRightSetpoint);
             })
      .Until([this] {
        return timer.Get() >= leftProfile.Duration() &&
               timer.Get() >= rightProfile.Duration();
      });
}
