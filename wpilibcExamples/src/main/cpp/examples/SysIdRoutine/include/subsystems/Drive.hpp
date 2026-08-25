// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include "Constants.hpp"
#include "wpi/commands2/SubsystemBase.hpp"
#include "wpi/commands2/sysid/SysIdRoutine.hpp"
#include "wpi/drive/DifferentialDrive.hpp"
#include "wpi/drivers/motor/PWMSparkMax.hpp"
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
  wpi::PWMSparkMax leftMotor{constants::drive::LEFT_MOTOR1_PORT};
  wpi::PWMSparkMax rightMotor{constants::drive::RIGHT_MOTOR1_PORT};
  wpi::DifferentialDrive drive{
      [this](auto val) { leftMotor.SetThrottle(val); },
      [this](auto val) { rightMotor.SetThrottle(val); }};

  wpi::Encoder leftEncoder{constants::drive::LEFT_ENCODER_PORTS[0],
                           constants::drive::LEFT_ENCODER_PORTS[1],
                           constants::drive::LEFT_ENCODER_REVERSED};

  wpi::Encoder rightEncoder{constants::drive::RIGHT_ENCODER_PORTS[0],
                            constants::drive::RIGHT_ENCODER_PORTS[1],
                            constants::drive::RIGHT_ENCODER_REVERSED};

  wpi::cmd::sysid::SysIdRoutine sysIdRoutine{
      wpi::cmd::sysid::Config{std::nullopt, std::nullopt, std::nullopt,
                              nullptr},
      wpi::cmd::sysid::Mechanism{
          [this](wpi::units::volts<> driveVoltage) {
            leftMotor.SetVoltage(driveVoltage);
            rightMotor.SetVoltage(driveVoltage);
          },
          [this](wpi::sysid::SysIdRoutineLog* log) {
            log->Motor("drive-left")
                .voltage(leftMotor.GetThrottle() *
                         wpi::RobotController::GetBatteryVoltage())
                .position(wpi::units::meters<>{leftEncoder.GetDistance()})
                .velocity(
                    wpi::units::meters_per_second<>{leftEncoder.GetRate()});
            log->Motor("drive-right")
                .voltage(rightMotor.GetThrottle() *
                         wpi::RobotController::GetBatteryVoltage())
                .position(wpi::units::meters<>{rightEncoder.GetDistance()})
                .velocity(
                    wpi::units::meters_per_second<>{rightEncoder.GetRate()});
          },
          this}};
};
