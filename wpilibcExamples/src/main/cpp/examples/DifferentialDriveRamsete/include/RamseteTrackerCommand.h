/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <utility>

#include <frc/Timer.h>
#include <frc/controller/RamseteController.h>
#include <frc/trajectory/Trajectory.h>
#include <frc2/command/CommandBase.h>
#include <frc2/command/CommandHelper.h>

#include "Drivetrain.h"

class RamseteTrackerCommand
    : public frc2::CommandHelper<frc2::CommandBase, RamseteTrackerCommand> {
 public:
  RamseteTrackerCommand(frc::Trajectory trajectory,
                        const frc::RamseteController& controller,
                        Drivetrain* drivetrain);

  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;

  void End(bool interrupted) override;

 private:
  frc::Trajectory m_trajectory;
  frc::RamseteController m_controller;

  Drivetrain* m_drivetrain;

  // Timer to keep track of time elapsed.
  frc::Timer m_timer;
};
