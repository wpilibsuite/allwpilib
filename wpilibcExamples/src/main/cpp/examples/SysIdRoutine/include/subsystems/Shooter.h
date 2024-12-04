// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <frc/Encoder.h>
#include <frc/RobotController.h>
#include <frc/controller/PIDController.h>
#include <frc/controller/SimpleMotorFeedforward.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc2/command/SubsystemBase.h>
#include <frc2/command/sysid/SysIdRoutine.h>

#include "Constants.h"

class Shooter : public frc2::SubsystemBase {
 public:
  Shooter();

  frc2::CommandPtr RunShooterCommand(std::function<double()> shooterSpeed);
  frc2::CommandPtr SysIdQuasistatic(frc2::sysid::Direction direction);
  frc2::CommandPtr SysIdDynamic(frc2::sysid::Direction direction);

 private:
  frc::PWMSparkMax m_shooterMotor{constants::shooter::kShooterMotorPort};
  frc::PWMSparkMax m_feederMotor{constants::shooter::kFeederMotorPort};

  frc::Encoder m_shooterEncoder{constants::shooter::kEncoderPorts[0],
                                constants::shooter::kEncoderPorts[1],
                                constants::shooter::kEncoderReversed};

  frc2::sysid::SysIdRoutine m_sysIdRoutine{
      frc2::sysid::Config{std::nullopt, std::nullopt, std::nullopt, nullptr},
      frc2::sysid::Mechanism{
          [this](units::volt_t driveVoltage) {
            m_shooterMotor.SetVoltage(driveVoltage);
          },
          [this](frc::sysid::SysIdRoutineLog* log) {
            log->Motor("shooter-wheel")
                .voltage(m_shooterMotor.Get() *
                         frc::RobotController::GetBatteryVoltage())
                .position(units::turn_t{m_shooterEncoder.GetDistance()})
                .velocity(
                    units::turns_per_second_t{m_shooterEncoder.GetRate()});
          },
          this}};
  frc::PIDController m_shooterFeedback{constants::shooter::kP, 0, 0};
  frc::SimpleMotorFeedforward<units::radians> m_shooterFeedforward{
      constants::shooter::kS, constants::shooter::kV, constants::shooter::kA};
};
