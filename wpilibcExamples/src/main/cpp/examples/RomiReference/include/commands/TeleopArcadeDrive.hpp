// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include "subsystems/Drivetrain.hpp"
#include "wpi/commands2/Command.hpp"
#include "wpi/commands2/CommandHelper.hpp"

class TeleopArcadeDrive
    : public wpi::cmd::CommandHelper<wpi::cmd::Command, TeleopArcadeDrive> {
 public:
  /**
   * Creates a new ArcadeDrive. This command will drive your robot according to
   * the velocity suppliers. This command does not terminate.
   *
   * @param drivetrain The drivetrain subsystem on which this command will run
   * @param xaxisVelocitySupplier Supplier of forward/backward velocity
   * @param zaxisRotateSupplier Supplier of rotational velocity
   */
  TeleopArcadeDrive(Drivetrain* subsystem,
                    std::function<double()> xaxisVelocitySupplier,
                    std::function<double()> zaxisRotateSupplier);
  void Execute() override;

 private:
  Drivetrain* m_drive;
  std::function<double()> m_xaxisVelocitySupplier;
  std::function<double()> m_zaxisRotateSupplier;
};
