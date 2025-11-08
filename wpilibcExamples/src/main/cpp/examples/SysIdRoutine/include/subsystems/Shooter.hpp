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

  wpi::cmd::CommandPtr RunShooterCommand(std::function<double()> shooterSpeed);
  wpi::cmd::CommandPtr SysIdQuasistatic(wpi::cmd::sysid::Direction direction);
  wpi::cmd::CommandPtr SysIdDynamic(wpi::cmd::sysid::Direction direction);

 private:
  wpi::PWMSparkMax m_shooterMotor{constants::shooter::kShooterMotorPort};
  wpi::PWMSparkMax m_feederMotor{constants::shooter::kFeederMotorPort};

  wpi::Encoder m_shooterEncoder{constants::shooter::kEncoderPorts[0],
                                constants::shooter::kEncoderPorts[1],
                                constants::shooter::kEncoderReversed};

  wpi::cmd::sysid::SysIdRoutine m_sysIdRoutine{
      wpi::cmd::sysid::Config{std::nullopt, std::nullopt, std::nullopt,
                              nullptr},
      wpi::cmd::sysid::Mechanism{
          [this](wpi::units::volt_t driveVoltage) {
            m_shooterMotor.SetVoltage(driveVoltage);
          },
          [this](wpi::sysid::SysIdRoutineLog* log) {
            log->Motor("shooter-wheel")
                .voltage(m_shooterMotor.Get() *
                         wpi::RobotController::GetBatteryVoltage())
                .position(wpi::units::turn_t{m_shooterEncoder.GetDistance()})
                .velocity(
                    wpi::units::turns_per_second_t{m_shooterEncoder.GetRate()});
          },
          this}};
  wpi::math::PIDController m_shooterFeedback{constants::shooter::kP, 0, 0};
  wpi::math::SimpleMotorFeedforward<wpi::units::radians> m_shooterFeedforward{
      constants::shooter::kS, constants::shooter::kV, constants::shooter::kA};
};
