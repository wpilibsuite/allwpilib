// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/TeleopArcadeDrive.hpp"

#include "subsystems/Drivetrain.hpp"

TeleopArcadeDrive::TeleopArcadeDrive(
    Drivetrain* subsystem, std::function<double()> xaxisVelocitySupplier,
    std::function<double()> zaxisRotateSuppplier)
    : drive{subsystem},
      xaxisVelocitySupplier{xaxisVelocitySupplier},
      zaxisRotateSupplier{zaxisRotateSuppplier} {
  AddRequirements(subsystem);
}

void TeleopArcadeDrive::Execute() {
  drive->ArcadeDrive(xaxisVelocitySupplier(), zaxisRotateSupplier());
}
