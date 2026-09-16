// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "Constants.hpp"
#include "wpi/drivers/motor/PWMSparkMax.hpp"
#include "wpi/hardware/pneumatic/DoubleSolenoid.hpp"

class Intake {
 public:
  void Deploy();
  void Retract();
  void Activate(double velocity);
  bool IsDeployed() const;

 private:
  wpi::PWMSparkMax motor{IntakeConstants::MOTOR_PORT};
  wpi::DoubleSolenoid piston{
      wpi::CANPort::CAN_S0, wpi::PneumaticsModuleType::CTRE_PCM,
      IntakeConstants::PISTON_FWD_CHANNEL, IntakeConstants::PISTON_REV_CHANNEL};
};
