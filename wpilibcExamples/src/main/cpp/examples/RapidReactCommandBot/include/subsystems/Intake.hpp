// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include "Constants.hpp"
#include "wpi/commands2/CommandPtr.hpp"
#include "wpi/commands2/SubsystemBase.hpp"
#include "wpi/drivers/motor/PWMSparkMax.hpp"
#include "wpi/hardware/pneumatic/DoubleSolenoid.hpp"

class Intake : public wpi::cmd::SubsystemBase {
 public:
  Intake() = default;

  /** Returns a command that deploys the intake, and then runs the intake motor
   * indefinitely. */
  wpi::cmd::CommandPtr IntakeCommand();

  /** Returns a command that turns off and retracts the intake. */
  wpi::cmd::CommandPtr RetractCommand();

 private:
  wpi::PWMSparkMax motor{IntakeConstants::MOTOR_PORT};

  // Double solenoid connected to two channels of a PCM with the default CAN ID
  wpi::DoubleSolenoid piston{
      wpi::CANPort::CAN_S0, wpi::PneumaticsModuleType::CTRE_PCM,
      IntakeConstants::SOLENOID_PORTS[0], IntakeConstants::SOLENOID_PORTS[1]};
};
