// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Storage.h"

Storage::Storage() {
  SetDefaultCommand(
      RunOnce([this] { m_motor.Disable(); }).AndThen([] {}).WithName("Idle"));
}

frc2::CommandPtr Storage::RunCommand() {
  return Run([this] { m_motor.Set(1.0); }).WithName("Run");
}
