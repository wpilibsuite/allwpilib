// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <frc/Encoder.h>
#include <frc/RobotController.h>
#include <frc/drive/DifferentialDrive.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc2/command/SubsystemBase.h>
#include <frc2/command/sysid/SysIdRoutine.h>

#include "Constants.h"

class Drive : public frc2::SubsystemBase {
 public:
  Drive();

  frc2::CommandPtr ArcadeDriveCommand(std::function<double()> fwd,
                                      std::function<double()> rot);
  frc2::CommandPtr SysIdQuasistatic(frc2::sysid::Direction direction);
  frc2::CommandPtr SysIdDynamic(frc2::sysid::Direction direction);

 private:
  frc::PWMSparkMax m_leftMotor{constants::drive::kLeftMotor1Port};
  frc::PWMSparkMax m_rightMotor{constants::drive::kRightMotor1Port};
  frc::DifferentialDrive m_drive{[this](auto val) { m_leftMotor.Set(val); },
                                 [this](auto val) { m_rightMotor.Set(val); }};

  frc::Encoder m_leftEncoder{constants::drive::kLeftEncoderPorts[0],
                             constants::drive::kLeftEncoderPorts[1],
                             constants::drive::kLeftEncoderReversed};

  frc::Encoder m_rightEncoder{constants::drive::kRightEncoderPorts[0],
                              constants::drive::kRightEncoderPorts[1],
                              constants::drive::kRightEncoderReversed};

  frc2::sysid::SysIdRoutine m_sysIdRoutine{
      frc2::sysid::Config{std::nullopt, std::nullopt, std::nullopt, nullptr},
      frc2::sysid::Mechanism{
          [this](units::volt_t driveVoltage) {
            m_leftMotor.SetVoltage(driveVoltage);
            m_rightMotor.SetVoltage(driveVoltage);
          },
          [this](frc::sysid::SysIdRoutineLog* log) {
            log->Motor("drive-left")
                .voltage(m_leftMotor.Get() *
                         frc::RobotController::GetBatteryVoltage())
                .position(units::meter_t{m_leftEncoder.GetDistance()})
                .velocity(units::meters_per_second_t{m_leftEncoder.GetRate()});
            log->Motor("drive-right")
                .voltage(m_rightMotor.Get() *
                         frc::RobotController::GetBatteryVoltage())
                .position(units::meter_t{m_rightEncoder.GetDistance()})
                .velocity(units::meters_per_second_t{m_rightEncoder.GetRate()});
          },
          this}};
};
