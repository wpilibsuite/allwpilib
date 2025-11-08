// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <frc2/command/Command.h>
#include <frc2/command/CommandHelper.h>

#include "subsystems/Drivetrain.h"

class TeleopArcadeDrive
    : public frc2::CommandHelper<frc2::Command, TeleopArcadeDrive> {
 public:
  /**
   * Creates a new ArcadeDrive. This command will drive your robot according to
   * the speed suppliers. This command does not terminate.
   *
   * @param drivetrain The drivetrain subsystem on which this command will run
   * @param xaxisSpeedSupplier Supplier of forward/backward speed
   * @param zaxisRotateSupplier Supplier of rotational speed
   */
  TeleopArcadeDrive(Drivetrain* subsystem,
                    std::function<double()> xaxisSpeedSupplier,
                    std::function<double()> zaxisRotateSupplier);
  void Execute() override;

 private:
  Drivetrain* m_drive;
  std::function<double()> m_xaxisSpeedSupplier;
  std::function<double()> m_zaxisRotateSupplier;
};
