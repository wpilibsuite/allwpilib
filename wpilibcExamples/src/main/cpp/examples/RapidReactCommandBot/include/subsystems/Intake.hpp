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

class Intake : public frc2::SubsystemBase {
 public:
  Intake() = default;

  /** Returns a command that deploys the intake, and then runs the intake motor
   * indefinitely. */
  frc2::CommandPtr IntakeCommand();

  /** Returns a command that turns off and retracts the intake. */
  frc2::CommandPtr RetractCommand();

 private:
  frc::PWMSparkMax m_motor{IntakeConstants::kMotorPort};

  // Double solenoid connected to two channels of a PCM with the default CAN ID
  frc::DoubleSolenoid m_piston{0, frc::PneumaticsModuleType::CTREPCM,
                               IntakeConstants::kSolenoidPorts[0],
                               IntakeConstants::kSolenoidPorts[1]};
};
