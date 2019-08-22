/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc2/command/CommandHelper.h>
#include <frc2/command/WaitCommand.h>

#include "subsystems/Claw.h"

/**
 * Opens the claw for one second. Real robots should use sensors, stalling
 * motors is BAD!
 */
class OpenClaw : public frc2::CommandHelper<frc2::WaitCommand, OpenClaw> {
 public:
  explicit OpenClaw(Claw* claw);
  void Initialize() override;
  void End(bool interrupted) override;

 private:
  Claw* m_claw;
};
