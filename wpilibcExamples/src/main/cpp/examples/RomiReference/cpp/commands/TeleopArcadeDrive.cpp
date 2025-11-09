// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/TeleopArcadeDrive.hpp"

#include <utility>

#include "subsystems/Drivetrain.hpp"

TeleopArcadeDrive::TeleopArcadeDrive(
    Drivetrain* subsystem, std::function<double()> xaxisSpeedSupplier,
    std::function<double()> zaxisRotateSuppplier)
    : m_drive{subsystem},
      m_xaxisSpeedSupplier{std::move(xaxisSpeedSupplier)},
      m_zaxisRotateSupplier{std::move(zaxisRotateSuppplier)} {
  AddRequirements(subsystem);
}

void TeleopArcadeDrive::Execute() {
  m_drive->ArcadeDrive(m_xaxisSpeedSupplier(), m_zaxisRotateSupplier());
}
