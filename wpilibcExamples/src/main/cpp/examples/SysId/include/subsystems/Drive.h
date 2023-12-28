// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <frc/Encoder.h>
#include <frc/RobotController.h>
#include <frc/drive/DifferentialDrive.h>
#include <frc/motorcontrol/MotorControllerGroup.h>
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
  frc::PWMSparkMax leftMotor1{constants::drive::kLeftMotor1Port};
  frc::PWMSparkMax leftMotor2{constants::drive::kLeftMotor2Port};

  frc::MotorControllerGroup m_leftMotors{leftMotor1, leftMotor2};

  frc::PWMSparkMax rightMotor1{constants::drive::kRightMotor1Port};
  frc::PWMSparkMax rightMotor2{constants::drive::kRightMotor2Port};

  frc::MotorControllerGroup m_rightMotors{rightMotor1, rightMotor2};

  frc::DifferentialDrive m_drive{m_leftMotors, m_rightMotors};

  frc::Encoder m_leftEncoder{constants::drive::kLeftEncoderPorts[0],
                             constants::drive::kLeftEncoderPorts[1],
                             constants::drive::kLeftEncoderReversed};

  frc::Encoder m_rightEncoder{constants::drive::kRightEncoderPorts[0],
                              constants::drive::kRightEncoderPorts[1],
                              constants::drive::kRightEncoderReversed};

  units::volt_t m_appliedVoltage{0_V};
  units::meter_t m_distance{0_m};
  units::meters_per_second_t m_velocity{0_mps};

  frc2::sysid::SysIdRoutine m_sysIdRoutine{
      frc2::sysid::Config{std::nullopt, std::nullopt, std::nullopt,
                          std::nullopt},
      frc2::sysid::Mechanism{
          [this](units::volt_t driveVoltage) {
            m_leftMotors.SetVoltage(driveVoltage);
            m_rightMotors.SetVoltage(driveVoltage);
          },
          [this](frc::sysid::MotorLog* log) {
            log->RecordFrameLinear(
                m_appliedVoltage = m_leftMotors.Get() *
                                   frc::RobotController::GetBatteryVoltage(),
                m_distance = units::meter_t{m_leftEncoder.GetDistance()},
                m_velocity =
                    units::meters_per_second_t{m_leftEncoder.GetRate()},
                "drive-left");
            log->RecordFrameLinear(
                m_appliedVoltage = m_rightMotors.Get() *
                                   frc::RobotController::GetBatteryVoltage(),
                m_distance = units::meter_t{m_rightEncoder.GetDistance()},
                m_velocity =
                    units::meters_per_second_t{m_rightEncoder.GetRate()},
                "drive-right");
          },
          this}};
};
