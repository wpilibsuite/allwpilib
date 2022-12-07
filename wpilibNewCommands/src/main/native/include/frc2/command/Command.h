// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <initializer_list>
#include <memory>
#include <span>
#include <string>

#include <units/time.h>
#include <wpi/Demangle.h>
#include <wpi/SmallSet.h>
#include <wpi/deprecated.h>

#include "frc2/command/Subsystem.h"

namespace frc2 {

template <typename T>
std::string GetTypeName(const T& type) {
  return wpi::Demangle(typeid(type).name());
}

class PerpetualCommand;

/**
 * A state machine representing a complete action to be performed by the robot.
 * Commands are run by the CommandScheduler, and can be composed into
 * CommandGroups to allow users to build complicated multi-step actions without
 * the need to roll the state machine logic themselves.
 *
 * <p>Commands are run synchronously from the main robot loop; no
 * multithreading is used, unless specified explicitly from the command
 * implementation.
 *
 * <p>Note: ALWAYS create a subclass by extending CommandHelper<Base, Subclass>,
 * or decorators will not function!
 *
 * This class is provided by the NewCommands VendorDep
 *
 * @see CommandScheduler
 * @see CommandHelper
 */
class Command {
 public:
  Command() = default;
  virtual ~Command();

  Command(const Command&) = default;
  Command& operator=(const Command& rhs);
  Command(Command&&) = default;
  Command& operator=(Command&&) = default;

  /**
   * The initial subroutine of a command.  Called once when the command is
   * initially scheduled.
   */
  virtual void Initialize();

  /**
   * The main body of a command.  Called repeatedly while the command is
   * scheduled.
   */
  virtual void Execute();

  /**
   * The action to take when the command ends.  Called when either the command
   * finishes normally, or when it interrupted/canceled.
   *
   * @param interrupted whether the command was interrupted/canceled
   */
  virtual void End(bool interrupted);

  /**
   * Whether the command has finished.  Once a command finishes, the scheduler
   * will call its end() method and un-schedule it.
   *
   * @return whether the command has finished.
   */
  virtual bool IsFinished() { return false; }

  /**
   * Specifies the set of subsystems used by this command. Two commands cannot
   * use the same subsystem at the same time. If another command is scheduled
   * that shares a requirement, GetInterruptionBehavior() will be checked and
   * followed. If no subsystems are required, return an empty set.
   *
   * <p>Note: it is recommended that user implementations contain the
   * requirements as a field, and return that field here, rather than allocating
   * a new set every time this is called.
   *
   * @return the set of subsystems that are required
   * @see InterruptionBehavior
   */
  virtual wpi::SmallSet<Subsystem*, 4> GetRequirements() const = 0;

  /**
   * An enum describing the command's behavior when another command with a
   * shared requirement is scheduled.
   */
  enum class InterruptionBehavior {
    /**
     * This command ends, End(true) is called, and the incoming command is
     * scheduled normally.
     *
     * <p>This is the default behavior.
     */
    kCancelSelf,
    /** This command continues, and the incoming command is not scheduled. */
    kCancelIncoming
  };

  friend class CommandPtr;

  /**
   * Decorates this command with a timeout.  If the specified timeout is
   * exceeded before the command finishes normally, the command will be
   * interrupted and un-scheduled.  Note that the timeout only applies to the
   * command returned by this method; the calling command is not itself changed.
   *
   * @param duration the timeout duration
   * @return the command with the timeout added
   */
  [[nodiscard]] CommandPtr WithTimeout(units::second_t duration) &&;

  /**
   * Decorates this command with an interrupt condition.  If the specified
   * condition becomes true before the command finishes normally, the command
   * will be interrupted and un-scheduled. Note that this only applies to the
   * command returned by this method; the calling command is not itself changed.
   *
   * @param condition the interrupt condition
   * @return the command with the interrupt condition added
   */
  [[nodiscard]] CommandPtr Until(std::function<bool()> condition) &&;

  /**
   * Decorates this command with an interrupt condition.  If the specified
   * condition becomes true before the command finishes normally, the command
   * will be interrupted and un-scheduled. Note that this only applies to the
   * command returned by this method; the calling command is not itself changed.
   *
   * @param condition the interrupt condition
   * @return the command with the interrupt condition added
   * @deprecated Replace with Until()
   */
  WPI_DEPRECATED("Replace with Until()")
  [[nodiscard]] CommandPtr WithInterrupt(std::function<bool()> condition) &&;

  /**
   * Decorates this command with a runnable to run before this command starts.
   *
   * @param toRun the Runnable to run
   * @param requirements the required subsystems
   * @return the decorated command
   */
  [[nodiscard]] CommandPtr BeforeStarting(
      std::function<void()> toRun,
      std::initializer_list<Subsystem*> requirements) &&;

  /**
   * Decorates this command with a runnable to run before this command starts.
   *
   * @param toRun the Runnable to run
   * @param requirements the required subsystems
   * @return the decorated command
   */
  [[nodiscard]] CommandPtr BeforeStarting(
      std::function<void()> toRun,
      std::span<Subsystem* const> requirements = {}) &&;

  /**
   * Decorates this command with a runnable to run after the command finishes.
   *
   * @param toRun the Runnable to run
   * @param requirements the required subsystems
   * @return the decorated command
   */
  [[nodiscard]] CommandPtr AndThen(
      std::function<void()> toRun,
      std::initializer_list<Subsystem*> requirements) &&;

  /**
   * Decorates this command with a runnable to run after the command finishes.
   *
   * @param toRun the Runnable to run
   * @param requirements the required subsystems
   * @return the decorated command
   */
  [[nodiscard]] CommandPtr AndThen(
      std::function<void()> toRun,
      std::span<Subsystem* const> requirements = {}) &&;

  /**
   * Decorates this command to run perpetually, ignoring its ordinary end
   * conditions.  The decorated command can still be interrupted or canceled.
   *
   * @return the decorated command
   * @deprecated PerpetualCommand violates the assumption that execute() doesn't
get called after isFinished() returns true -- an assumption that should be
valid. This was unsafe/undefined behavior from the start, and RepeatCommand
provides an easy way to achieve similar end results with slightly different (and
safe) semantics.
   */
  WPI_DEPRECATED(
      "PerpetualCommand violates the assumption that execute() doesn't get "
      "called after isFinished() returns true -- an assumption that should be "
      "valid."
      "This was unsafe/undefined behavior from the start, and RepeatCommand "
      "provides an easy way to achieve similar end results with slightly "
      "different (and safe) semantics.")
  PerpetualCommand Perpetually() &&;

  /**
   * Decorates this command to run repeatedly, restarting it when it ends, until
   * this command is interrupted. The decorated command can still be canceled.
   *
   * @return the decorated command
   */
  [[nodiscard]] CommandPtr Repeatedly() &&;

  /**
   * Decorates this command to run "by proxy" by wrapping it in a
   * ProxyCommand. This is useful for "forking off" from command groups
   * when the user does not wish to extend the command's requirements to the
   * entire command group.
   *
   * <p>This overload transfers command ownership to the returned CommandPtr.
   *
   * @return the decorated command
   */
  [[nodiscard]] CommandPtr AsProxy() &&;

  /**
   * Decorates this command to only run if this condition is not met. If the
   * command is already running and the condition changes to true, the command
   * will not stop running. The requirements of this command will be kept for
   * the new conditonal command.
   *
   * @param condition the condition that will prevent the command from running
   * @return the decorated command
   */
  [[nodiscard]] CommandPtr Unless(std::function<bool()> condition) &&;

  /**
   * Decorates this command to run or stop when disabled.
   *
   * @param doesRunWhenDisabled true to run when disabled.
   * @return the decorated command
   */
  [[nodiscard]] CommandPtr IgnoringDisable(bool doesRunWhenDisabled) &&;

  /**
   * Decorates this command to run or stop when disabled.
   *
   * @param interruptBehavior true to run when disabled.
   * @return the decorated command
   */
  [[nodiscard]] CommandPtr WithInterruptBehavior(
      Command::InterruptionBehavior interruptBehavior) &&;

  /**
   * Decorates this command with a lambda to call on interrupt or end, following
   * the command's inherent Command::End(bool) method.
   *
   * @param end a lambda accepting a boolean parameter specifying whether the
   * command was interrupted.
   * @return the decorated command
   */
  [[nodiscard]] CommandPtr FinallyDo(std::function<void(bool)> end) &&;

  /**
   * Decorates this command with a lambda to call on interrupt, following the
   * command's inherent Command::End(bool) method.
   *
   * @param handler a lambda to run when the command is interrupted
   * @return the decorated command
   */
  [[nodiscard]] CommandPtr HandleInterrupt(std::function<void()> handler) &&;

  /**
   * Decorates this Command with a name. Is an inline function for
   * #SetName(std::string_view);
   *
   * @param name name
   * @return the decorated Command
   */
  [[nodiscard]] CommandPtr WithName(std::string_view name) &&;

  /**
   * Schedules this command.
   */
  void Schedule();

  /**
   * Cancels this command. Will call End(true). Commands will be canceled
   * regardless of interruption behavior.
   */
  void Cancel();

  /**
   * Whether or not the command is currently scheduled.  Note that this does not
   * detect whether the command is being run by a CommandGroup, only whether it
   * is directly being run by the scheduler.
   *
   * @return Whether the command is scheduled.
   */
  bool IsScheduled() const;

  /**
   * Whether the command requires a given subsystem.  Named "HasRequirement"
   * rather than "requires" to avoid confusion with Command::Requires(Subsystem)
   * -- this may be able to be changed in a few years.
   *
   * @param requirement the subsystem to inquire about
   * @return whether the subsystem is required
   */
  bool HasRequirement(Subsystem* requirement) const;

  /**
   * Whether the command is currently grouped in a command group.  Used as extra
   * insurance to prevent accidental independent use of grouped commands.
   */
  bool IsGrouped() const;

  /**
   * Sets whether the command is currently grouped in a command group.  Can be
   * used to "reclaim" a command if a group is no longer going to use it.  NOT
   * ADVISED!
   */
  void SetGrouped(bool grouped);

  /**
   * Whether the given command should run when the robot is disabled.  Override
   * to return true if the command should run when disabled.
   *
   * @return whether the command should run when the robot is disabled
   */
  virtual bool RunsWhenDisabled() const { return false; }

  /**
   * How the command behaves when another command with a shared requirement is
   * scheduled.
   *
   * @return a variant of InterruptionBehavior, defaulting to kCancelSelf.
   */
  virtual InterruptionBehavior GetInterruptionBehavior() const {
    return InterruptionBehavior::kCancelSelf;
  }

  /**
   * Gets the name of this Command. Defaults to the simple class name if not
   * overridden.
   *
   * @return The display name of the Command
   */
  virtual std::string GetName() const;

  /**
   * Sets the name of this Command. Nullop if not overridden.
   *
   * @param name The display name of the Command.
   */
  virtual void SetName(std::string_view name);

  /**
   * Transfers ownership of this command to a unique pointer.  Used for
   * decorator methods.
   */
  virtual CommandPtr ToPtr() && = 0;

 protected:
  /**
   * Transfers ownership of this command to a unique pointer.  Used for
   * decorator methods.
   */
  virtual std::unique_ptr<Command> TransferOwnership() && = 0;

  bool m_isGrouped = false;
};

/**
 * Checks if two commands have disjoint requirement sets.
 *
 * @param first The first command to check.
 * @param second The second command to check.
 * @return False if first and second share a requirement.
 */
bool RequirementsDisjoint(Command* first, Command* second);
}  // namespace frc2
