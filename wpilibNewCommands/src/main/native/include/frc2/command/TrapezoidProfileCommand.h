/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>
#include <initializer_list>

#include <frc/trajectory/TrapezoidProfile.h>
#include <frc2/Timer.h>

#include "frc2/command/CommandBase.h"
#include "frc2/command/CommandHelper.h"

namespace frc2 {
/**
 * A command that runs a TrapezoidProfile.  Useful for smoothly controlling
 * mechanism motion.
 *
 * @see TrapezoidProfile
 */
class TrapezoidProfileCommand
    : public CommandHelper<CommandBase, TrapezoidProfileCommand> {
 public:
  /**
   * Creates a new TrapezoidProfileCommand that will execute the given
   * TrapezoidalProfile. Output will be piped to the provided consumer function.
   *
   * @param profile The motion profile to execute.
   * @param output  The consumer for the profile output.
   */
  TrapezoidProfileCommand(
      frc::TrapezoidProfile profile,
      std::function<void(frc::TrapezoidProfile::State)> output,
      std::initializer_list<Subsystem*> requirements);

  void Initialize() override;

  void Execute() override;

  void End(bool interrupted) override;

  bool IsFinished() override;

 private:
  frc::TrapezoidProfile m_profile;
  std::function<void(frc::TrapezoidProfile::State)> m_output;

  Timer m_timer;
};

}  // namespace frc2
