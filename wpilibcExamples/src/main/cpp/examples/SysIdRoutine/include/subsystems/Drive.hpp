// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include "Constants.hpp"
#include "wpi/commands2/SubsystemBase.hpp"
#include "wpi/commands2/sysid/SysIdRoutine.hpp"
#include "wpi/drive/DifferentialDrive.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/system/RobotController.hpp"

class Drive : public wpi::cmd::SubsystemBase {
 public:
  Drive();

  wpi::cmd::CommandPtr ArcadeDriveCommand(std::function<double()> fwd,
                                          std::function<double()> rot);
  wpi::cmd::CommandPtr SysIdQuasistatic(wpi::cmd::sysid::Direction direction);
  wpi::cmd::CommandPtr SysIdDynamic(wpi::cmd::sysid::Direction direction);

 private:
  wpi::PWMSparkMax m_leftMotor{constants::drive::kLeftMotor1Port};
  wpi::PWMSparkMax m_rightMotor{constants::drive::kRightMotor1Port};
  wpi::DifferentialDrive m_drive{
      [this](auto val) { m_leftMotor.SetDutyCycle(val); },
      [this](auto val) { m_rightMotor.SetDutyCycle(val); }};

  wpi::Encoder m_leftEncoder{constants::drive::kLeftEncoderPorts[0],
                             constants::drive::kLeftEncoderPorts[1],
                             constants::drive::kLeftEncoderReversed};

  wpi::Encoder m_rightEncoder{constants::drive::kRightEncoderPorts[0],
                              constants::drive::kRightEncoderPorts[1],
                              constants::drive::kRightEncoderReversed};

  wpi::cmd::sysid::SysIdRoutine m_sysIdRoutine{
      wpi::cmd::sysid::Config{std::nullopt, std::nullopt, std::nullopt,
                              nullptr},
      wpi::cmd::sysid::Mechanism{
          [this](wpi::units::volt_t driveVoltage) {
            m_leftMotor.SetVoltage(driveVoltage);
            m_rightMotor.SetVoltage(driveVoltage);
          },
          [this](wpi::sysid::SysIdRoutineLog* log) {
            log->Motor("drive-left")
                .voltage(m_leftMotor.GetDutyCycle() *
                         wpi::RobotController::GetBatteryVoltage())
                .position(wpi::units::meter_t{m_leftEncoder.GetDistance()})
                .velocity(
                    wpi::units::meters_per_second_t{m_leftEncoder.GetRate()});
            log->Motor("drive-right")
                .voltage(m_rightMotor.GetDutyCycle() *
                         wpi::RobotController::GetBatteryVoltage())
                .position(wpi::units::meter_t{m_rightEncoder.GetDistance()})
                .velocity(
                    wpi::units::meters_per_second_t{m_rightEncoder.GetRate()});
          },
          this}};
};
