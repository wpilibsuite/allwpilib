// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include "Constants.hpp"
#include "wpi/commands2/SubsystemBase.hpp"
#include "wpi/commands2/sysid/SysIdRoutine.hpp"
#include "wpi/drivers/motor/PWMSparkMax.hpp"
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
  wpi::PWMSparkMax shooterMotor{constants::shooter::SHOOTER_MOTOR_PORT};
  wpi::PWMSparkMax feederMotor{constants::shooter::FEEDER_MOTOR_PORT};

  wpi::Encoder shooterEncoder{constants::shooter::ENCODER_PORTS[0],
                              constants::shooter::ENCODER_PORTS[1],
                              constants::shooter::ENCODER_REVERSED};

  wpi::cmd::sysid::SysIdRoutine sysIdRoutine{
      wpi::cmd::sysid::Config{std::nullopt, std::nullopt, std::nullopt,
                              nullptr},
      wpi::cmd::sysid::Mechanism{
          [this](wpi::units::volts<> driveVoltage) {
            shooterMotor.SetVoltage(driveVoltage);
          },
          [this](wpi::sysid::SysIdRoutineLog* log) {
            log->Motor("shooter-wheel")
                .voltage(shooterMotor.GetThrottle() *
                         wpi::RobotController::GetBatteryVoltage())
                .position(wpi::units::turns<>{shooterEncoder.GetDistance()})
                .velocity(
                    wpi::units::turns_per_second<>{shooterEncoder.GetRate()});
          },
          this}};
  wpi::math::PIDController shooterFeedback{constants::shooter::kP, 0, 0};
  wpi::math::SimpleMotorFeedforward<wpi::units::radians_> shooterFeedforward{
      constants::shooter::kS, constants::shooter::kV, constants::shooter::kA};
};
