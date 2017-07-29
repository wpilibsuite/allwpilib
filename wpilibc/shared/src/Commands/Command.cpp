/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Commands/Command.h"

#include <typeinfo>

#include "Commands/CommandGroup.h"
#include "Commands/Scheduler.h"
#include "RobotState.h"
#include "Timer.h"
#include "WPIErrors.h"

static const std::string kName = "name";
static const std::string kRunning = "running";
static const std::string kIsParented = "isParented";

int frc::Command::m_commandCounter = 0;

/**
 * Creates a new command.
 * The name of this command will be default.
 */
frc::Command::Command() : Command("", -1.0) {}

/**
 * Creates a new command with the given name and no timeout.
 *
 * @param name the name for this command
 */
frc::Command::Command(const std::string& name) : Command(name, -1.0) {}

/**
 * Creates a new command with the given timeout and a default name.
 *
 * @param timeout the time (in seconds) before this command "times out"
 * @see Command#isTimedOut() isTimedOut()
 */
frc::Command::Command(double timeout) : Command("", timeout) {}

/**
 * Creates a new command with the given name and timeout.
 *
 * @param name    the name of the command
 * @param timeout the time (in seconds) before this command "times out"
 * @see Command#isTimedOut() isTimedOut()
 */
frc::Command::Command(const std::string& name, double timeout) {
  // We use -1.0 to indicate no timeout.
  if (timeout < 0.0 && timeout != -1.0)
    wpi_setWPIErrorWithContext(ParameterOutOfRange, "timeout < 0.0");

  m_timeout = timeout;

  // If name contains an empty string
  if (name.length() == 0) {
    m_name = std::string("Command_") + std::string(typeid(*this).name());
  } else {
    m_name = name;
  }
}

frc::Command::~Command() {
  if (m_table != nullptr) m_table->RemoveTableListener(this);
}

/**
 * Get the ID (sequence number) for this command.
 *
 * The ID is a unique sequence number that is incremented for each command.
 *
 * @return the ID of this command
 */
int frc::Command::GetID() const { return m_commandID; }

/**
 * Sets the timeout of this command.
 *
 * @param timeout the timeout (in seconds)
 * @see Command#isTimedOut() isTimedOut()
 */
void frc::Command::SetTimeout(double timeout) {
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
double frc::Command::TimeSinceInitialized() const {
  if (m_startTime < 0.0)
    return 0.0;
  else
    return Timer::GetFPGATimestamp() - m_startTime;
}

/**
 * This method specifies that the given {@link Subsystem} is used by this
 * command.
 *
 * This method is crucial to the functioning of the Command System in general.
 *
 * <p>Note that the recommended way to call this method is in the
 * constructor.</p>
 *
 * @param subsystem the {@link Subsystem} required
 * @see Subsystem
 */
void frc::Command::Requires(Subsystem* subsystem) {
  if (!AssertUnlocked("Can not add new requirement to command")) return;

  if (subsystem != nullptr)
    m_requirements.insert(subsystem);
  else
    wpi_setWPIErrorWithContext(NullParameter, "subsystem");
}

/**
 * Called when the command has been removed.
 *
 * This will call {@link Command#interrupted() interrupted()} or
 * {@link Command#end() end()}.
 */
void frc::Command::Removed() {
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
  if (m_table != nullptr) m_table->PutBoolean(kRunning, false);
}

/**
 * Starts up the command.  Gets the command ready to start.
 *
 * <p>Note that the command will eventually start, however it will not
 * necessarily do so immediately, and may in fact be canceled before initialize
 * is even called.</p>
 */
void frc::Command::Start() {
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
 * @return whether or not the command should stay within the {@link Scheduler}.
 */
bool frc::Command::Run() {
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
void frc::Command::Initialize() {}

/**
 * The execute method is called repeatedly until this Command either finishes
 * or is canceled.
 */
void frc::Command::Execute() {}

/**
 * Called when the command ended peacefully.  This is where you may want
 * to wrap up loose ends, like shutting off a motor that was being used
 * in the command.
 */
void frc::Command::End() {}

/**
 * Called when the command ends because somebody called
 * {@link Command#cancel() cancel()} or another command shared the same
 * requirements as this one, and booted it out.
 *
 * <p>This is where you may want to wrap up loose ends, like shutting off a
 * motor that was being used in the command.</p>
 *
 * <p>Generally, it is useful to simply call the {@link Command#end() end()}
 * method within this method, as done here.</p>
 */
void frc::Command::Interrupted() { End(); }

void frc::Command::_Initialize() {}

void frc::Command::_Interrupted() {}

void frc::Command::_Execute() {}

void frc::Command::_End() {}

/**
 * Called to indicate that the timer should start.
 *
 * This is called right before {@link Command#initialize() initialize()} is,
 * inside the {@link Command#run() run()} method.
 */
void frc::Command::StartTiming() { m_startTime = Timer::GetFPGATimestamp(); }

/**
 * Returns whether or not the
 * {@link Command#timeSinceInitialized() timeSinceInitialized()} method returns
 * a number which is greater than or equal to the timeout for the command.
 *
 * If there is no timeout, this will always return false.
 *
 * @return whether the time has expired
 */
bool frc::Command::IsTimedOut() const {
  return m_timeout != -1 && TimeSinceInitialized() >= m_timeout;
}

/**
 * Returns the requirements (as an std::set of {@link Subsystem Subsystems}
 * pointers) of this command.
 *
 * @return the requirements (as an std::set of {@link Subsystem Subsystems}
 *         pointers) of this command
 */
frc::Command::SubsystemSet frc::Command::GetRequirements() const {
  return m_requirements;
}

/**
 * Prevents further changes from being made.
 */
void frc::Command::LockChanges() { m_locked = true; }

/**
 * If changes are locked, then this will generate a CommandIllegalUse error.
 *
 * @param message the message to report on error (it is appended by a default
 *                message)
 * @return true if assert passed, false if assert failed
 */
bool frc::Command::AssertUnlocked(const std::string& message) {
  if (m_locked) {
    std::string buf =
        message + " after being started or being added to a command group";
    wpi_setWPIErrorWithContext(CommandIllegalUse, buf);
    return false;
  }
  return true;
}

/**
 * Sets the parent of this command.  No actual change is made to the group.
 *
 * @param parent the parent
 */
void frc::Command::SetParent(CommandGroup* parent) {
  if (parent == nullptr) {
    wpi_setWPIErrorWithContext(NullParameter, "parent");
  } else if (m_parent != nullptr) {
    wpi_setWPIErrorWithContext(CommandIllegalUse,
                               "Can not give command to a command group after "
                               "already being put in a command group");
  } else {
    LockChanges();
    m_parent = parent;
    if (m_table != nullptr) {
      m_table->PutBoolean(kIsParented, true);
    }
  }
}

/**
 * Clears list of subsystem requirements. This is only used by
 * {@link ConditionalCommand} so cancelling the chosen command works properly in
 * {@link CommandGroup}.
 */
void frc::Command::ClearRequirements() { m_requirements.clear(); }

/**
 * This is used internally to mark that the command has been started.
 *
 * The lifecycle of a command is:
 *
 * startRunning() is called.
 * run() is called (multiple times potentially)
 * removed() is called
 *
 * It is very important that startRunning and removed be called in order or some
 * assumptions of the code will be broken.
 */
void frc::Command::StartRunning() {
  m_running = true;
  m_startTime = -1;
  if (m_table != nullptr) m_table->PutBoolean(kRunning, true);
}

/**
 * Returns whether or not the command is running.
 *
 * This may return true even if the command has just been canceled, as it may
 * not have yet called {@link Command#interrupted()}.
 *
 * @return whether or not the command is running
 */
bool frc::Command::IsRunning() const { return m_running; }

/**
 * This will cancel the current command.
 *
 * <p>This will cancel the current command eventually.  It can be called
 * multiple times. And it can be called when the command is not running.  If
 * the command is running though, then the command will be marked as canceled
 * and eventually removed.</p>
 *
 * <p>A command can not be canceled if it is a part of a command group, you
 * must cancel the command group instead.</p>
 */
void frc::Command::Cancel() {
  if (m_parent != nullptr)
    wpi_setWPIErrorWithContext(
        CommandIllegalUse,
        "Can not cancel a command that is part of a command group");

  _Cancel();
}

/**
 * This works like cancel(), except that it doesn't throw an exception if it is
 * a part of a command group.
 *
 * Should only be called by the parent command group.
 */
void frc::Command::_Cancel() {
  if (IsRunning()) m_canceled = true;
}

/**
 * Returns whether or not this has been canceled.
 *
 * @return whether or not this has been canceled
 */
bool frc::Command::IsCanceled() const { return m_canceled; }

/**
 * Returns whether or not this command can be interrupted.
 *
 * @return whether or not this command can be interrupted
 */
bool frc::Command::IsInterruptible() const { return m_interruptible; }

/**
 * Sets whether or not this command can be interrupted.
 *
 * @param interruptible whether or not this command can be interrupted
 */
void frc::Command::SetInterruptible(bool interruptible) {
  m_interruptible = interruptible;
}

/**
 * Checks if the command requires the given {@link Subsystem}.
 *
 * @param system the system
 * @return whether or not the subsystem is required (false if given nullptr)
 */
bool frc::Command::DoesRequire(Subsystem* system) const {
  return m_requirements.count(system) > 0;
}

/**
 * Returns the {@link CommandGroup} that this command is a part of.
 *
 * Will return null if this {@link Command} is not in a group.
 *
 * @return the {@link CommandGroup} that this command is a part of (or null if
 *         not in group)
 */
frc::CommandGroup* frc::Command::GetGroup() const { return m_parent; }

/**
 * Sets whether or not this {@link Command} should run when the robot is
 * disabled.
 *
 * <p>By default a command will not run when the robot is disabled, and will in
 * fact be canceled.</p>
 *
 * @param run whether or not this command should run when the robot is disabled
 */
void frc::Command::SetRunWhenDisabled(bool run) { m_runWhenDisabled = run; }

/**
 * Returns whether or not this {@link Command} will run when the robot is
 * disabled, or if it will cancel itself.
 *
 * @return whether or not this {@link Command} will run when the robot is
 *         disabled, or if it will cancel itself
 */
bool frc::Command::WillRunWhenDisabled() const { return m_runWhenDisabled; }

std::string frc::Command::GetName() const { return m_name; }

std::string frc::Command::GetSmartDashboardType() const { return "Command"; }

void frc::Command::InitTable(std::shared_ptr<ITable> subtable) {
  if (m_table != nullptr) m_table->RemoveTableListener(this);
  m_table = subtable;
  if (m_table != nullptr) {
    m_table->PutString(kName, GetName());
    m_table->PutBoolean(kRunning, IsRunning());
    m_table->PutBoolean(kIsParented, m_parent != nullptr);
    m_table->AddTableListener(kRunning, this, false);
  }
}

std::shared_ptr<ITable> frc::Command::GetTable() const { return m_table; }

void frc::Command::ValueChanged(ITable* source, llvm::StringRef key,
                                std::shared_ptr<nt::Value> value, bool isNew) {
  if (!value->IsBoolean()) return;
  if (value->GetBoolean()) {
    if (!IsRunning()) Start();
  } else {
    if (IsRunning()) Cancel();
  }
}
