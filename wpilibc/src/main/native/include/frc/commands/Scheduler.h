/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include "frc/ErrorBase.h"
#include "frc/smartdashboard/SendableBase.h"

namespace frc {

class ButtonScheduler;
class Command;
class Subsystem;

class Scheduler : public ErrorBase, public SendableBase {
 public:
  /**
   * Returns the Scheduler, creating it if one does not exist.
   *
   * @return the Scheduler
   */
  static Scheduler* GetInstance();

  /**
   * Add a command to be scheduled later.
   *
   * In any pass through the scheduler, all commands are added to the additions
   * list, then at the end of the pass, they are all scheduled.
   *
   * @param command The command to be scheduled
   */
  void AddCommand(Command* command);

  void AddButton(ButtonScheduler* button);

  /**
   * Registers a Subsystem to this Scheduler, so that the Scheduler might know
   * if a default Command needs to be run.
   *
   * All Subsystems should call this.
   *
   * @param system the system
   */
  void RegisterSubsystem(Subsystem* subsystem);

  /**
   * Runs a single iteration of the loop.
   *
   * This method should be called often in order to have a functioning
   * Command system. The loop has five stages:
   *
   * <ol>
   *   <li>Poll the Buttons</li>
   *   <li>Execute/Remove the Commands</li>
   *   <li>Send values to SmartDashboard</li>
   *   <li>Add Commands</li>
   *   <li>Add Defaults</li>
   * </ol>
   */
  void Run();

  /**
   * Removes the Command from the Scheduler.
   *
   * @param command the command to remove
   */
  void Remove(Command* command);

  void RemoveAll();

  /**
   * Completely resets the scheduler. Undefined behavior if running.
   */
  void ResetAll();

  void SetEnabled(bool enabled);

  void InitSendable(SendableBuilder& builder) override;

 private:
  Scheduler();
  ~Scheduler() override;

  Scheduler(Scheduler&&) = default;
  Scheduler& operator=(Scheduler&&) = default;

  struct Impl;
  std::unique_ptr<Impl> m_impl;
};

}  // namespace frc
