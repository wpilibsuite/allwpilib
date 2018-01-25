/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <set>
#include <string>

#include <llvm/Twine.h>

#include "ErrorBase.h"
#include "SmartDashboard/SendableBase.h"

namespace frc {

class CommandGroup;
class Subsystem;

/**
 * The Command class is at the very core of the entire command framework.
 *
 * Every command can be started with a call to Start(). Once a command is
 * started it will call Initialize(), and then will repeatedly call Execute()
 * until the IsFinished() returns true. Once it does,End() will be called.
 *
 * However, if at any point while it is running Cancel() is called, then the
 * command will be stopped and Interrupted() will be called.
 *
 * If a command uses a Subsystem, then it should specify that it does so by
 * calling the Requires() method in its constructor. Note that a Command may
 * have multiple requirements, and Requires() should be called for each one.
 *
 * If a command is running and a new command with shared requirements is
 * started, then one of two things will happen. If the active command is
 * interruptible, then Cancel() will be called and the command will be removed
 * to make way for the new one. If the active command is not interruptible, the
 * other one will not even be started, and the active one will continue
 * functioning.
 *
 * @see CommandGroup
 * @see Subsystem
 */
class Command : public ErrorBase, public SendableBase {
  friend class CommandGroup;
  friend class Scheduler;

 public:
  Command();
  explicit Command(const llvm::Twine& name);
  explicit Command(double timeout);
  Command(const llvm::Twine& name, double timeout);
  ~Command() override = default;
  double TimeSinceInitialized() const;
  void Requires(Subsystem* s);
  bool IsCanceled() const;
  void Start();
  bool Run();
  void Cancel();
  bool IsRunning() const;
  bool IsInitialized() const;
  bool IsCompleted() const;
  bool IsInterruptible() const;
  void SetInterruptible(bool interruptible);
  bool DoesRequire(Subsystem* subsystem) const;
  typedef std::set<Subsystem*> SubsystemSet;
  SubsystemSet GetRequirements() const;
  CommandGroup* GetGroup() const;
  void SetRunWhenDisabled(bool run);
  bool WillRunWhenDisabled() const;
  int GetID() const;

 protected:
  void SetTimeout(double timeout);
  bool IsTimedOut() const;
  bool AssertUnlocked(const std::string& message);
  void SetParent(CommandGroup* parent);
  bool IsParented() const;
  void ClearRequirements();

  virtual void Initialize();
  virtual void Execute();

  /**
   * Returns whether this command is finished.
   *
   * If it is, then the command will be removed and End() will be called.
   *
   * It may be useful for a team to reference the IsTimedOut() method for
   * time-sensitive commands.
   *
   * Returning false will result in the command never ending automatically.
   * It may still be cancelled manually or interrupted by another command.
   * Returning true will result in the command executing once and finishing
   * immediately. We recommend using InstantCommand for this.
   *
   * @return Whether this command is finished.
   * @see IsTimedOut()
   */
  virtual bool IsFinished() = 0;

  virtual void End();
  virtual void Interrupted();

  virtual void _Initialize();
  virtual void _Interrupted();
  virtual void _Execute();
  virtual void _End();
  virtual void _Cancel();

  friend class ConditionalCommand;

 private:
  void LockChanges();
  void Removed();
  void StartRunning();
  void StartTiming();

  // The time since this command was initialized
  double m_startTime = -1;

  // The time (in seconds) before this command "times out" (-1 if no timeout)
  double m_timeout;

  // Whether or not this command has been initialized
  bool m_initialized = false;

  // The requirements (or null if no requirements)
  SubsystemSet m_requirements;

  // Whether or not it is running
  bool m_running = false;

  // Whether or not it is interruptible
  bool m_interruptible = true;

  // Whether or not it has been canceled
  bool m_canceled = false;

  // Whether or not it has been locked
  bool m_locked = false;

  // Whether this command should run when the robot is disabled
  bool m_runWhenDisabled = false;

  // The CommandGroup this is in
  CommandGroup* m_parent = nullptr;

  // Whether or not this command has completed running
  bool m_completed = false;

  int m_commandID = m_commandCounter++;
  static int m_commandCounter;

 public:
  void InitSendable(SendableBuilder& builder) override;
};

}  // namespace frc
