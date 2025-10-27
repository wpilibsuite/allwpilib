// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <wpi/commands2/CommandPtr.hpp>
#include <wpi/commands2/SubsystemBase.hpp>
#include <wpi/hardware/motor/PWMSparkMax.hpp>
#include <wpi/hardware/pneumatic/DoubleSolenoid.hpp>

#include "Constants.hpp"

class Intake : public wpi::cmd::SubsystemBase {
 public:
  Intake() = default;

  /** Returns a command that deploys the intake, and then runs the intake motor
   * indefinitely. */
  wpi::cmd::CommandPtr IntakeCommand();

  /** Returns a command that turns off and retracts the intake. */
  wpi::cmd::CommandPtr RetractCommand();

 private:
  wpi::PWMSparkMax m_motor{IntakeConstants::kMotorPort};

  // Double solenoid connected to two channels of a PCM with the default CAN ID
  wpi::DoubleSolenoid m_piston{0, wpi::PneumaticsModuleType::CTREPCM,
                               IntakeConstants::kSolenoidPorts[0],
                               IntakeConstants::kSolenoidPorts[1]};
};
