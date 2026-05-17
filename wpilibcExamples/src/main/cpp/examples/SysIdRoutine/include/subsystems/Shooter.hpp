// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include "Constants.hpp"
#include "wpi/commands2/SubsystemBase.hpp"
#include "wpi/commands2/sysid/SysIdRoutine.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/math/controller/PIDController.hpp"
#include "wpi/math/controller/SimpleMotorFeedforward.hpp"
#include "wpi/system/RobotController.hpp"

class Shooter : public wpi::cmd::SubsystemBase {
 public:
  Shooter();

  wpi::cmd::CommandPtr RunShooterCommand(
      std::function<double()> shooterVelocity);
  wpi::cmd::CommandPtr SysIdQuasistatic(wpi::cmd::sysid::Direction direction);
  wpi::cmd::CommandPtr SysIdDynamic(wpi::cmd::sysid::Direction direction);

 private:
  wpi::PWMSparkMax shooterMotor{constants::shooter::kShooterMotorPort};
  wpi::PWMSparkMax feederMotor{constants::shooter::kFeederMotorPort};

  wpi::Encoder shooterEncoder{constants::shooter::kEncoderPorts[0],
                              constants::shooter::kEncoderPorts[1],
                              constants::shooter::kEncoderReversed};

  wpi::cmd::sysid::SysIdRoutine sysIdRoutine{
      wpi::cmd::sysid::Config{std::nullopt, std::nullopt, std::nullopt,
                              nullptr},
      wpi::cmd::sysid::Mechanism{
          [this](wpi::units::volt_t driveVoltage) {
            shooterMotor.SetVoltage(driveVoltage);
          },
          [this](wpi::sysid::SysIdRoutineLog* log) {
            log->Motor("shooter-wheel")
                .voltage(shooterMotor.GetThrottle() *
                         wpi::RobotController::GetBatteryVoltage())
                .position(wpi::units::turn_t{shooterEncoder.GetDistance()})
                .velocity(
                    wpi::units::turns_per_second_t{shooterEncoder.GetRate()});
          },
          this}};
  wpi::math::PIDController shooterFeedback{constants::shooter::kP, 0, 0};
  wpi::math::SimpleMotorFeedforward<wpi::units::radians> shooterFeedforward{
      constants::shooter::kS, constants::shooter::kV, constants::shooter::kA};
};
