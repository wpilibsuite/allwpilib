// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/Encoder.h>
#include <frc/controller/SimpleMotorFeedforward.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc2/command/PIDSubsystem.h>
#include <units/angle.h>

class ShooterSubsystem : public frc2::PIDSubsystem {
 public:
  ShooterSubsystem();

  void UseOutput(double output, double setpoint) override;

  double GetMeasurement() override;

  bool AtSetpoint();

  void RunFeeder();

  void StopFeeder();

 private:
  frc::PWMSparkMax m_shooterMotor;
  frc::PWMSparkMax m_feederMotor;
  frc::Encoder m_shooterEncoder;
  frc::SimpleMotorFeedforward<units::turns> m_shooterFeedforward;
};
