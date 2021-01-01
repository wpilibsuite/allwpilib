// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/TeleopArcadeDrive.h"

#include "subsystems/Drivetrain.h"

TeleopArcadeDrive::TeleopArcadeDrive(
    Drivetrain* subsystem, std::function<double()> xaxisSpeedSupplier,
    std::function<double()> zaxisRotateSuppplier)
    : m_drive{subsystem},
      m_xaxisSpeedSupplier{xaxisSpeedSupplier},
      m_zaxisRotateSupplier{zaxisRotateSuppplier} {
  AddRequirements({subsystem});
}

void TeleopArcadeDrive::Execute() {
  m_drive->ArcadeDrive(m_xaxisSpeedSupplier(), m_zaxisRotateSupplier());
}
