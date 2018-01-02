/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Commands/Command.h"

#include <typeinfo>

#include "Commands/CommandGroup.h"
#include "Commands/Scheduler.h"
#include "LiveWindow/LiveWindow.h"
#include "RobotState.h"
#include "SmartDashboard/SendableBuilder.h"
#include "Timer.h"
#include "WPIErrors.h"

using namespace frc;

int Command::m_commandCounter = 0;

/**
 * Creates a new command.
 *
 * The name of this command will be default.
 */
Command::Command() : Command("", -1.0) {}

/**
 * Creates a new command with the given name and no timeout.
 *
 * @param name the name for this command
 */
Command::Command(const llvm::Twine& name) : Command(name, -1.0) {}

/**
 * Creates a new command with the given timeout and a default name.
 *
 * @param timeout the time (in seconds) before this command "times out"
 * @see IsTimedOut()
 */
Command::Command(double timeout) : Command("", timeout) {}

/**
 * Creates a new command with the given name and timeout.
 *
 * @param name    the name of the command
 * @param timeout the time (in seconds) before this command "times out"
 * @see IsTimedOut()
 */
Command::Command(const llvm::Twine& name, double timeout)
    : SendableBase(false) {
  // We use -1.0 to indicate no timeout.
  if (timeout < 0.0 && timeout != -1.0)
    wpi_setWPIErrorWithContext(ParameterOutOfRange, "timeout < 0.0");

  m_timeout = timeout;

  // If name contains an empty string
  if (name.isTriviallyEmpty() ||
      (name.isSingleStringRef() && name.getSingleStringRef().empty())) {
    SetName("Command_" + llvm::Twine(typeid(*this).name()));
  } else {
    SetName(name);
  }
}

/**
 * Get the ID (sequence number) for this command.
 *
 * The ID is a unique sequence number that is incremented for each command.
 *
 * @return The ID of this command
 */
int Command::GetID() const { return m_commandID; }

/**
 * Sets the timeout of this command.
 *
 * @param timeout the timeout (in seconds)
 * @see IsTimedOut()
 */
void Command::SetTimeout(double timeout) {
  if (timeout < 0.0)
    wpi_setWPIErrorWithContext(ParameterOutOfRange, "timeout < 0.0");
  else
    m_timeout = timeout;
}

/**
 * Returns the time since this command was initialized (in seconds).
 *
 * This function will work even if there is no specified timeout.
 *
 * @return the time since this command was initialized (in seconds).
 */
double Command::TimeSinceInitialized() const {
  if (m_startTime < 0.0)
    return 0.0;
  else
    return Timer::GetFPGATimestamp() - m_startTime;
}

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
void Command::Requires(Subsystem* subsystem) {
  if (!AssertUnlocked("Can not add new requirement to command")) return;

  if (subsystem != nullptr)
    m_requirements.insert(subsystem);
  else
    wpi_setWPIErrorWithContext(NullParameter, "subsystem");
}

/**
 * Called when the command has been removed.
 *
 * This will call Interrupted() or End().
 */
void Command::Removed() {
  if (m_initialized) {
    if (IsCanceled()) {
      Interrupted();
      _Interrupted();
    } else {
      End();
      _End();
    }
  }
  m_initialized = false;
  m_canceled = false;
  m_running = false;
}

/**
 * Starts up the command. Gets the command ready to start.
 *
 * Note that the command will eventually start, however it will not necessarily
 * do so immediately, and may in fact be canceled before initialize is even
 * called.
 */
void Command::Start() {
  LockChanges();
  if (m_parent != nullptr)
    wpi_setWPIErrorWithContext(
        CommandIllegalUse,
        "Can not start a command that is part of a command group");

  Scheduler::GetInstance()->AddCommand(this);
}

/**
 * The run method is used internally to actually run the commands.
 *
 * @return Whether or not the command should stay within the Scheduler.
 */
bool Command::Run() {
  if (!m_runWhenDisabled && m_parent == nullptr && RobotState::IsDisabled())
    Cancel();

  if (IsCanceled()) return false;

  if (!m_initialized) {
    m_initialized = true;
    StartTiming();
    _Initialize();
    Initialize();
  }
  _Execute();
  Execute();
  return !IsFinished();
}

/**
 * The initialize method is called the first time this Command is run after
 * being started.
 */
void Command::Initialize() {}

/**
 * The execute method is called repeatedly until this Command either finishes
 * or is canceled.
 */
void Command::Execute() {}

/**
 * Called when the command ended peacefully. This is where you may want to wrap
 * up loose ends, like shutting off a motor that was being used in the command.
 */
void Command::End() {}

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
void Command::Interrupted() { End(); }

void Command::_Initialize() {}

void Command::_Interrupted() {}

void Command::_Execute() {}

void Command::_End() {}

/**
 * Called to indicate that the timer should start.
 *
 * This is called right before Initialize() is, inside the Run() method.
 */
void Command::StartTiming() { m_startTime = Timer::GetFPGATimestamp(); }

/**
 * Returns whether or not the TimeSinceInitialized() method returns a number
 * which is greater than or equal to the timeout for the command.
 *
 * If there is no timeout, this will always return false.
 *
 * @return whether the time has expired
 */
bool Command::IsTimedOut() const {
  return m_timeout != -1 && TimeSinceInitialized() >= m_timeout;
}

/**
 * Returns the requirements (as an std::set of Subsystem pointers) of this
 * command.
 *
 * @return The requirements (as an std::set of Subsystem pointers) of this
 *         command
 */
Command::SubsystemSet Command::GetRequirements() const {
  return m_requirements;
}

/**
 * Prevents further changes from being made.
 */
void Command::LockChanges() { m_locked = true; }

/**
 * If changes are locked, then this will generate a CommandIllegalUse error.
 *
 * @param message The message to report on error (it is appended by a default
 *                message)
 * @return True if assert passed, false if assert failed.
 */
bool Command::AssertUnlocked(const std::string& message) {
  if (m_locked) {
    std::string buf =
        message + " after being started or being added to a command group";
    wpi_setWPIErrorWithContext(CommandIllegalUse, buf);
    return false;
  }
  return true;
}

/**
 * Sets the parent of this command. No actual change is made to the group.
 *
 * @param parent the parent
 */
void Command::SetParent(CommandGroup* parent) {
  if (parent == nullptr) {
    wpi_setWPIErrorWithContext(NullParameter, "parent");
  } else if (m_parent != nullptr) {
    wpi_setWPIErrorWithContext(CommandIllegalUse,
                               "Can not give command to a command group after "
                               "already being put in a command group");
  } else {
    LockChanges();
    m_parent = parent;
  }
}

/**
 * Returns whether the command has a parent.
 *
 * @param True if the command has a parent.
 */
bool Command::IsParented() const { return m_parent != nullptr; }

/**
 * Clears list of subsystem requirements.
 *
 * This is only used by ConditionalCommand so cancelling the chosen command
 * works properly in CommandGroup.
 */
void Command::ClearRequirements() { m_requirements.clear(); }

/**
 * This is used internally to mark that the command has been started.
 *
 * The lifecycle of a command is:
 *
 * StartRunning() is called. Run() is called (multiple times potentially).
 * Removed() is called.
 *
 * It is very important that StartRunning() and Removed() be called in order or
 * some assumptions of the code will be broken.
 */
void Command::StartRunning() {
  m_running = true;
  m_startTime = -1;
}

/**
 * Returns whether or not the command is running.
 *
 * This may return true even if the command has just been canceled, as it may
 * not have yet called Interrupted().
 *
 * @return whether or not the command is running
 */
bool Command::IsRunning() const { return m_running; }

/**
 * This will cancel the current command.
 *
 * This will cancel the current command eventually. It can be called multiple
 * times. And it can be called when the command is not running. If the command
 * is running though, then the command will be marked as canceled and eventually
 * removed.
 *
 * A command can not be canceled if it is a part of a command group, you must
 * cancel the command group instead.
 */
void Command::Cancel() {
  if (m_parent != nullptr)
    wpi_setWPIErrorWithContext(
        CommandIllegalUse,
        "Can not cancel a command that is part of a command group");

  _Cancel();
}

/**
 * This works like Cancel(), except that it doesn't throw an exception if it is
 * a part of a command group.
 *
 * Should only be called by the parent command group.
 */
void Command::_Cancel() {
  if (IsRunning()) m_canceled = true;
}

/**
 * Returns whether or not this has been canceled.
 *
 * @return whether or not this has been canceled
 */
bool Command::IsCanceled() const { return m_canceled; }

/**
 * Returns whether or not this command can be interrupted.
 *
 * @return whether or not this command can be interrupted
 */
bool Command::IsInterruptible() const { return m_interruptible; }

/**
 * Sets whether or not this command can be interrupted.
 *
 * @param interruptible whether or not this command can be interrupted
 */
void Command::SetInterruptible(bool interruptible) {
  m_interruptible = interruptible;
}

/**
 * Checks if the command requires the given Subsystem.
 *
 * @param system the system
 * @return whether or not the subsystem is required (false if given nullptr)
 */
bool Command::DoesRequire(Subsystem* system) const {
  return m_requirements.count(system) > 0;
}

/**
 * Returns the CommandGroup that this command is a part of.
 *
 * Will return null if this Command is not in a group.
 *
 * @return The CommandGroup that this command is a part of (or null if not in
 *         group)
 */
CommandGroup* Command::GetGroup() const { return m_parent; }

/**
 * Sets whether or not this Command should run when the robot is disabled.
 *
 * By default a command will not run when the robot is disabled, and will in
 * fact be canceled.
 *
 * @param run Whether this command should run when the robot is disabled.
 */
void Command::SetRunWhenDisabled(bool run) { m_runWhenDisabled = run; }

/**
 * Returns whether or not this Command will run when the robot is disabled, or
 * if it will cancel itself.
 *
 * @return Whether this Command will run when the robot is disabled, or if it
 * will cancel itself.
 */
bool Command::WillRunWhenDisabled() const { return m_runWhenDisabled; }

void Command::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Command");
  builder.AddStringProperty(".name", [=]() { return GetName(); }, nullptr);
  builder.AddBooleanProperty("running", [=]() { return IsRunning(); },
                             [=](bool value) {
                               if (value) {
                                 if (!IsRunning()) Start();
                               } else {
                                 if (IsRunning()) Cancel();
                               }
                             });
  builder.AddBooleanProperty(".isParented", [=]() { return IsParented(); },
                             nullptr);
}
