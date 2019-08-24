/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "CommandBase.h"
#include "CommandHelper.h"

namespace frc2 {
/**
 * A command that runs a given runnable when it is initalized, and another
 * runnable when it ends. Useful for running and then stopping a motor, or
 * extending and then retracting a solenoid. Has no end condition as-is; either
 * subclass it or use Command.WithTimeout() or Command.InterruptOn() to give it
 * one.
 */
class StartEndCommand : public CommandHelper<CommandBase, StartEndCommand> {
 public:
  /**
   * Creates a new StartEndCommand.  Will run the given runnables when the
   * command starts and when it ends.
   *
   * @param onInit       the Runnable to run on command init
   * @param onEnd        the Runnable to run on command end
   * @param requirements the subsystems required by this command
   */
  StartEndCommand(std::function<void()> onInit, std::function<void()> onEnd,
                  std::initializer_list<Subsystem*> requirements);

  StartEndCommand(StartEndCommand&& other) = default;

  StartEndCommand(const StartEndCommand& other);

  void Initialize() override;

  void End(bool interrupted) override;

 protected:
  std::function<void()> m_onInit;
  std::function<void()> m_onEnd;
};
}  // namespace frc2
