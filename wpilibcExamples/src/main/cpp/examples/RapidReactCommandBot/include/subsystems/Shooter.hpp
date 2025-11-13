// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include "Constants.hpp"
#include "wpi/commands2/CommandPtr.hpp"
#include "wpi/commands2/SubsystemBase.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/math/controller/PIDController.hpp"
#include "wpi/math/controller/SimpleMotorFeedforward.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/angular_velocity.hpp"

class Shooter : public wpi::cmd::SubsystemBase {
 public:
  Shooter();

  /**
   * Returns a command to shoot the balls currently stored in the robot. Spins
   * the shooter flywheel up to the specified setpoint, and then runs the feeder
   * motor.
   *
   * @param setpointRotationsPerSecond The desired shooter velocity
   */
  wpi::cmd::CommandPtr ShootCommand(wpi::units::turns_per_second_t setpoint);

 private:
  wpi::PWMSparkMax m_shooterMotor{ShooterConstants::kShooterMotorPort};
  wpi::PWMSparkMax m_feederMotor{ShooterConstants::kFeederMotorPort};

  wpi::Encoder m_shooterEncoder{ShooterConstants::kEncoderPorts[0],
                                ShooterConstants::kEncoderPorts[1],
                                ShooterConstants::kEncoderReversed};
  wpi::math::SimpleMotorFeedforward<wpi::units::radians> m_shooterFeedforward{
      ShooterConstants::kS, ShooterConstants::kV};
  wpi::math::PIDController m_shooterFeedback{ShooterConstants::kP, 0.0, 0.0};
};
