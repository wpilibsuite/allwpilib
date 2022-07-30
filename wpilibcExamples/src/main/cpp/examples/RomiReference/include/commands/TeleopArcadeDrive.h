// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/command/CommandBase.h>
#include <frc/command/CommandHelper.h>

#include "subsystems/Drivetrain.h"

class TeleopArcadeDrive
    : public frc::CommandHelper<frc::CommandBase, TeleopArcadeDrive> {
 public:
  TeleopArcadeDrive(Drivetrain* subsystem,
                    std::function<double()> xaxisSpeedSupplier,
                    std::function<double()> zaxisRotateSupplier);
  void Execute() override;

 private:
  Drivetrain* m_drive;
  std::function<double()> m_xaxisSpeedSupplier;
  std::function<double()> m_zaxisRotateSupplier;
};
