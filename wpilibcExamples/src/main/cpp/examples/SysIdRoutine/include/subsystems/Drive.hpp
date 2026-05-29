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
  wpi::PWMSparkMax leftLeader{constants::drive::kLeftMotor1Port};
  wpi::PWMSparkMax rightLeader{constants::drive::kRightMotor1Port};
  wpi::PWMSparkMax leftFollower{constants::drive::kLeftMotor2Port};
  wpi::PWMSparkMax rightFollower{constants::drive::kRightMotor2Port};
  wpi::DifferentialDrive drive{
      [this](auto val) { leftLeader.SetThrottle(val); },
      [this](auto val) { rightLeader.SetThrottle(val); }};
  wpi::Encoder leftEncoder{constants::drive::kLeftEncoderPorts[0],
                           constants::drive::kLeftEncoderPorts[1],
                           constants::drive::kLeftEncoderReversed};

  wpi::Encoder rightEncoder{constants::drive::kRightEncoderPorts[0],
                            constants::drive::kRightEncoderPorts[1],
                            constants::drive::kRightEncoderReversed};

  wpi::cmd::sysid::SysIdRoutine sysIdRoutine{
      wpi::cmd::sysid::Config{std::nullopt, std::nullopt, std::nullopt,
                              nullptr},
      wpi::cmd::sysid::Mechanism{
          [this](wpi::units::volt_t driveVoltage) {
            leftLeader.SetVoltage(driveVoltage);
            rightLeader.SetVoltage(driveVoltage);
          },
          [this](wpi::sysid::SysIdRoutineLog* log) {
            log->Motor("drive-left")
                .voltage(leftLeader.GetThrottle() *
                         wpi::RobotController::GetBatteryVoltage())
                .position(wpi::units::meter_t{leftEncoder.GetDistance()})
                .velocity(
                    wpi::units::meters_per_second_t{leftEncoder.GetRate()});
            log->Motor("drive-right")
                .voltage(rightLeader.GetThrottle() *
                         wpi::RobotController::GetBatteryVoltage())
                .position(wpi::units::meter_t{rightEncoder.GetDistance()})
                .velocity(
                    wpi::units::meters_per_second_t{rightEncoder.GetRate()});
          },
          this}};
};
