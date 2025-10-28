// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/hardware/motor/PWMSparkMax.hpp>
#include <wpi/hardware/pneumatic/DoubleSolenoid.hpp>

#include "Constants.hpp"

class Intake {
 public:
  void Deploy();
  void Retract();
  void Activate(double speed);
  bool IsDeployed() const;

 private:
  wpi::PWMSparkMax m_motor{IntakeConstants::kMotorPort};
  wpi::DoubleSolenoid m_piston{0, wpi::PneumaticsModuleType::CTREPCM,
                               IntakeConstants::kPistonFwdChannel,
                               IntakeConstants::kPistonRevChannel};
};
