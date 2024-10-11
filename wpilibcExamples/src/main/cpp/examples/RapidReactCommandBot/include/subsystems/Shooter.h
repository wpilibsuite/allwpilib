// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <frc/Encoder.h>
#include <frc/controller/PIDController.h>
#include <frc/controller/SimpleMotorFeedforward.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc2/command/CommandPtr.h>
#include <frc2/command/SubsystemBase.h>
#include <units/angle.h>
#include <units/angular_velocity.h>

#include "Constants.h"

class Shooter : public frc2::SubsystemBase {
 public:
  Shooter();

  /**
   * Returns a command to shoot the balls currently stored in the robot. Spins
   * the shooter flywheel up to the specified setpoint, and then runs the feeder
   * motor.
   *
   * @param setpointRotationsPerSecond The desired shooter velocity
   */
  [[nodiscard]]
  frc2::CommandPtr ShootCommand(units::turns_per_second_t setpoint);

 private:
  frc::PWMSparkMax m_shooterMotor{ShooterConstants::kShooterMotorPort};
  frc::PWMSparkMax m_feederMotor{ShooterConstants::kFeederMotorPort};

  frc::Encoder m_shooterEncoder{ShooterConstants::kEncoderPorts[0],
                                ShooterConstants::kEncoderPorts[1],
                                ShooterConstants::kEncoderReversed};
  frc::SimpleMotorFeedforward<units::radians> m_shooterFeedforward{
      ShooterConstants::kS, ShooterConstants::kV};
  frc::PIDController m_shooterFeedback{ShooterConstants::kP, 0.0, 0.0};
};
