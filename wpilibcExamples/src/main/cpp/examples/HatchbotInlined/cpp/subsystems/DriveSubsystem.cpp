// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/DriveSubsystem.h"

#include <wpi/sendable/SendableBuilder.h>

using namespace DriveConstants;

DriveSubsystem::DriveSubsystem()
    : m_left1{LEFT_MOTOR_1_PORT},
      m_left2{LEFT_MOTOR_2_PORT},
      m_right1{RIGHT_MOTOR_1_PORT},
      m_right2{RIGHT_MOTOR_2_PORT},
      m_leftEncoder{LEFT_ENCODER_PORTS[0], LEFT_ENCODER_PORTS[1]},
      m_rightEncoder{RIGHT_ENCODER_PORTS[0], RIGHT_ENCODER_PORTS[1]} {
  wpi::SendableRegistry::AddChild(&m_drive, &m_left1);
  wpi::SendableRegistry::AddChild(&m_drive, &m_right1);

  m_left1.AddFollower(m_left2);
  m_right1.AddFollower(m_right2);

  // We need to invert one side of the drivetrain so that positive voltages
  // result in both sides moving forward. Depending on how your robot's
  // gearbox is constructed, you might have to invert the left side instead.
  m_right1.SetInverted(true);

  // Set the distance per pulse for the encoders
  m_leftEncoder.SetDistancePerPulse(ENCODER_DISTANCE_PER_PULSE);
  m_rightEncoder.SetDistancePerPulse(ENCODER_DISTANCE_PER_PULSE);
}

void DriveSubsystem::Periodic() {
  // Implementation of subsystem periodic method goes here.
}

void DriveSubsystem::ArcadeDrive(double fwd, double rot) {
  m_drive.ArcadeDrive(fwd, rot);
}

void DriveSubsystem::ResetEncoders() {
  m_leftEncoder.Reset();
  m_rightEncoder.Reset();
}

double DriveSubsystem::GetAverageEncoderDistance() {
  return (m_leftEncoder.GetDistance() + m_rightEncoder.GetDistance()) / 2.0;
}

void DriveSubsystem::SetMaxOutput(double maxOutput) {
  m_drive.SetMaxOutput(maxOutput);
}

void DriveSubsystem::InitSendable(wpi::SendableBuilder& builder) {
  SubsystemBase::InitSendable(builder);

  // Publish encoder distances to telemetry.
  builder.AddDoubleProperty(
      "leftDistance", [this] { return m_leftEncoder.GetDistance(); }, nullptr);
  builder.AddDoubleProperty(
      "rightDistance", [this] { return m_rightEncoder.GetDistance(); },
      nullptr);
}
