// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Storage.hpp"

Storage::Storage() {
  SetDefaultCommand(
      RunOnce([this] { m_motor.Disable(); }).AndThen([] {}).WithName("Idle"));
}

wpi::cmd::CommandPtr Storage::RunCommand() {
  return Run([this] { m_motor.SetDutyCycle(1.0); }).WithName("Run");
}
