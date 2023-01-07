// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <frc/DoubleSolenoid.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc2/command/CommandPtr.h>
#include <frc2/command/SubsystemBase.h>

#include "Constants.h"

class Intake : public frc2::SubsystemBase {
 public:
  Intake() = default;

  /** Returns a command that deploys the intake, and then runs the intake motor
   * indefinitely. */
  [[nodiscard]] frc2::CommandPtr IntakeCommand();

  /** Returns a command that turns off and retracts the intake. */
  [[nodiscard]] frc2::CommandPtr RetractCommand();

 private:
  frc::PWMSparkMax m_motor{IntakeConstants::kMotorPort};
  frc::DoubleSolenoid m_piston{frc::PneumaticsModuleType::REVPH,
                               IntakeConstants::kSolenoidPorts[0],
                               IntakeConstants::kSolenoidPorts[1]};
};
