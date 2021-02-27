// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>

#include <wpi/SmallPtrSet.h>
#include <wpi/Twine.h>

#include "frc/ErrorBase.h"
#include "frc/commands/Subsystem.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {

class CommandGroup;

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
class Command : public ErrorBase,
                public Sendable,
                public SendableHelper<Command> {
  friend class CommandGroup;
  friend class Scheduler;

 public:
  /**
   * Creates a new command.
   *
   * The name of this command will be default.
   */
  Command();

  /**
   * Creates a new command with the given name and no timeout.
   *
   * @param name the name for this command
   */
  explicit Command(const wpi::Twine& name);

  /**
   * Creates a new command with the given timeout and a default name.
   *
   * @param timeout the time (in seconds) before this command "times out"
   * @see IsTimedOut()
   */
  explicit Command(double timeout);

  /**
   * Creates a new command with the given timeout and a default name.
   *
   * @param subsystem the subsystem that the command requires
   */
  explicit Command(Subsystem& subsystem);

  /**
   * Creates a new command with the given name and timeout.
   *
   * @param name    the name of the command
   * @param timeout the time (in seconds) before this command "times out"
   * @see IsTimedOut()
   */
  Command(const wpi::Twine& name, double timeout);

  /**
   * Creates a new command with the given name and timeout.
   *
   * @param name      the name of the command
   * @param subsystem the subsystem that the command requires
   */
  Command(const wpi::Twine& name, Subsystem& subsystem);

  /**
   * Creates a new command with the given name and timeout.
   *
   * @param timeout   the time (in seconds) before this command "times out"
   * @param subsystem the subsystem that the command requires @see IsTimedOut()
   */
  Command(double timeout, Subsystem& subsystem);

  /**
   * Creates a new command with the given name and timeout.
   *
   * @param name      the name of the command
   * @param timeout   the time (in seconds) before this command "times out"
   * @param subsystem the subsystem that the command requires @see IsTimedOut()
   */
  Command(const wpi::Twine& name, double timeout, Subsystem& subsystem);

  ~Command() override = default;

  Command(Command&&) = default;
  Command& operator=(Command&&) = default;

  /**
   * Returns the time since this command was initialized (in seconds).
   *
   * This function will work even if there is no specified timeout.
   *
   * @return the time since this command was initialized (in seconds).
   */
  double TimeSinceInitialized() const;

  /**
   * This method specifies that the given Subsystem is used by this command.
   *
   * This method is crucial to the functioning of the Command System in general.
   *
   * Note that the recommended way to call this method is in the constructor.
   *
   * @param subsystem The Subsystem required
   * @see Subsystem
   */
  void Requires(Subsystem* s);

  /**
   * Starts up the command. Gets the command ready to start.
   *
   * Note that the command will eventually start, however it will not
   * necessarily do so immediately, and may in fact be canceled before
   * initialize is even called.
   */
  void Start();

  /**
   * The run method is used internally to actually run the commands.
   *
   * @return Whether or not the command should stay within the Scheduler.
   */
  bool Run();

  /**
   * This will cancel the current command.
   *
   * This will cancel the current command eventually. It can be called multiple
   * times. And it can be called when the command is not running. If the command
   * is running though, then the command will be marked as canceled and
   * eventually removed.
   *
   * A command can not be canceled if it is a part of a command group, you must
   * cancel the command group instead.
   */
  void Cancel();

  /**
   * Returns whether or not the command is running.
   *
   * This may return true even if the command has just been canceled, as it may
   * not have yet called Interrupted().
   *
   * @return whether or not the command is running
   */
  bool IsRunning() const;

  /**
   * Returns whether or not the command has been initialized.
   *
   * @return whether or not the command has been initialized.
   */
  bool IsInitialized() const;

  /**
   * Returns whether or not the command has completed running.
   *
   * @return whether or not the command has completed running.
   */
  bool IsCompleted() const;

  /**
   * Returns whether or not this has been canceled.
   *
   * @return whether or not this has been canceled
   */
  bool IsCanceled() const;

  /**
   * Returns whether or not this command can be interrupted.
   *
   * @return whether or not this command can be interrupted
   */
  bool IsInterruptible() const;

  /**
   * Sets whether or not this command can be interrupted.
   *
   * @param interruptible whether or not this command can be interrupted
   */
  void SetInterruptible(bool interruptible);

  /**
   * Checks if the command requires the given Subsystem.
   *
   * @param system the system
   * @return whether or not the subsystem is required (false if given nullptr)
   */
  bool DoesRequire(Subsystem* subsystem) const;

  using SubsystemSet = wpi::SmallPtrSetImpl<Subsystem*>;

  /**
   * Returns the requirements (as an std::set of Subsystem pointers) of this
   * command.
   *
   * @return The requirements (as an std::set of Subsystem pointers) of this
   *         command
   */
  const SubsystemSet& GetRequirements() const;

  /**
   * Returns the CommandGroup that this command is a part of.
   *
   * Will return null if this Command is not in a group.
   *
   * @return The CommandGroup that this command is a part of (or null if not in
   *         group)
   */
  CommandGroup* GetGroup() const;

  /**
   * Sets whether or not this Command should run when the robot is disabled.
   *
   * By default a command will not run when the robot is disabled, and will in
   * fact be canceled.
   *
   * @param run Whether this command should run when the robot is disabled.
   */
  void SetRunWhenDisabled(bool run);

  /**
   * Returns whether or not this Command will run when the robot is disabled, or
   * if it will cancel itself.
   *
   * @return Whether this Command will run when the robot is disabled, or if it
   * will cancel itself.
   */
  bool WillRunWhenDisabled() const;

  /**
   * Get the ID (sequence number) for this command.
   *
   * The ID is a unique sequence number that is incremented for each command.
   *
   * @return The ID of this command
   */
  int GetID() const;

 protected:
  /**
   * Sets the timeout of this command.
   *
   * @param timeout the timeout (in seconds)
   * @see IsTimedOut()
   */
  void SetTimeout(double timeout);

  /**
   * Returns whether or not the TimeSinceInitialized() method returns a number
   * which is greater than or equal to the timeout for the command.
   *
   * If there is no timeout, this will always return false.
   *
   * @return whether the time has expired
   */
  bool IsTimedOut() const;

  /**
   * If changes are locked, then this will generate a CommandIllegalUse error.
   *
   * @param message The message to report on error (it is appended by a default
   *                message)
   * @return True if assert passed, false if assert failed.
   */
  bool AssertUnlocked(const std::string& message);

  /**
   * Sets the parent of this command. No actual change is made to the group.
   *
   * @param parent the parent
   */
  void SetParent(CommandGroup* parent);

  /**
   * Returns whether the command has a parent.
   *
   * @param True if the command has a parent.
   */
  bool IsParented() const;

  /**
   * Clears list of subsystem requirements.
   *
   * This is only used by ConditionalCommand so canceling the chosen command
   * works properly in CommandGroup.
   */
  void ClearRequirements();

  /**
   * The initialize method is called the first time this Command is run after
   * being started.
   */
  virtual void Initialize();

  /**
   * The execute method is called repeatedly until this Command either finishes
   * or is canceled.
   */
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
   * It may still be canceled manually or interrupted by another command.
   * Returning true will result in the command executing once and finishing
   * immediately. We recommend using InstantCommand for this.
   *
   * @return Whether this command is finished.
   * @see IsTimedOut()
   */
  virtual bool IsFinished() = 0;

  /**
   * Called when the command ended peacefully.
   *
   * This is where you may want to wrap up loose ends, like shutting off a motor
   * that was being used in the command.
   */
  virtual void End();

  /**
   * Called when the command ends because somebody called Cancel() or another
   * command shared the same requirements as this one, and booted it out.
   *
   * This is where you may want to wrap up loose ends, like shutting off a motor
   * that was being used in the command.
   *
   * Generally, it is useful to simply call the End() method within this method,
   * as done here.
   */
  virtual void Interrupted();

  virtual void _Initialize();
  virtual void _Interrupted();
  virtual void _Execute();
  virtual void _End();

  /**
   * This works like Cancel(), except that it doesn't throw an exception if it
   * is a part of a command group.
   *
   * Should only be called by the parent command group.
   */
  virtual void _Cancel();

  friend class ConditionalCommand;

 public:
  /**
   * Gets the name of this Command.
   *
   * @return Name
   */
  std::string GetName() const;

  /**
   * Sets the name of this Command.
   *
   * @param name name
   */
  void SetName(const wpi::Twine& name);

  /**
   * Gets the subsystem name of this Command.
   *
   * @return Subsystem name
   */
  std::string GetSubsystem() const;

  /**
   * Sets the subsystem name of this Command.
   *
   * @param subsystem subsystem name
   */
  void SetSubsystem(const wpi::Twine& subsystem);

 private:
  /**
   * Prevents further changes from being made.
   */
  void LockChanges();

  /**
   * Called when the command has been removed.
   *
   * This will call Interrupted() or End().
   */
  void Removed();

  /**
   * This is used internally to mark that the command has been started.
   *
   * The lifecycle of a command is:
   *
   * StartRunning() is called. Run() is called (multiple times potentially).
   * Removed() is called.
   *
   * It is very important that StartRunning() and Removed() be called in order
   * or some assumptions of the code will be broken.
   */
  void StartRunning();

  /**
   * Called to indicate that the timer should start.
   *
   * This is called right before Initialize() is, inside the Run() method.
   */
  void StartTiming();

  // The time since this command was initialized
  double m_startTime = -1;

  // The time (in seconds) before this command "times out" (-1 if no timeout)
  double m_timeout;

  // Whether or not this command has been initialized
  bool m_initialized = false;

  // The requirements (or null if no requirements)
  wpi::SmallPtrSet<Subsystem*, 4> m_requirements;

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
