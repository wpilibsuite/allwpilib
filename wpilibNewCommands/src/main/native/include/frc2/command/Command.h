// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>

#include <units/time.h>
#include <wpi/Demangle.h>
#include <wpi/SmallSet.h>
#include <wpi/StackTrace.h>
#include <wpi/sendable/Sendable.h>

#include "frc2/command/Requirements.h"
#include "frc2/command/Subsystem.h"

namespace frc2 {

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
class Command : public wpi::Sendable, public wpi::SendableHelper<Command> {
 public:
  ~Command() override;

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
  virtual wpi::SmallSet<Subsystem*, 4> GetRequirements() const;

  /**
   * Adds the specified Subsystem requirements to the command.
   *
   * The scheduler will prevent two commands that require the same subsystem
   * from being scheduled simultaneously.
   *
   * Note that the scheduler determines the requirements of a command when it
   * is scheduled, so this method should normally be called from the command's
   * constructor.
   *
   * While this overload can be used with {@code AddRequirements({&subsystem1,
   * &subsystem2})}, {@code AddRequirements({&subsystem})} selects the {@code
   * AddRequirements(Subsystem*)} overload, which will function identically but
   * may cause warnings about redundant braces.
   *
   * @param requirements the Subsystem requirements to add, which can be
   * implicitly constructed from an initializer list or a span
   */
  void AddRequirements(Requirements requirements);

  /**
   * Adds the specified Subsystem requirements to the command.
   *
   * The scheduler will prevent two commands that require the same subsystem
   * from being scheduled simultaneously.
   *
   * Note that the scheduler determines the requirements of a command when it
   * is scheduled, so this method should normally be called from the command's
   * constructor.
   *
   * @param requirements the Subsystem requirements to add
   */
  void AddRequirements(wpi::SmallSet<Subsystem*, 4> requirements);

  /**
   * Adds the specified Subsystem requirement to the command.
   *
   * The scheduler will prevent two commands that require the same subsystem
   * from being scheduled simultaneously.
   *
   * Note that the scheduler determines the requirements of a command when it
   * is scheduled, so this method should normally be called from the command's
   * constructor.
   *
   * @param requirement the Subsystem requirement to add
   */
  void AddRequirements(Subsystem* requirement);

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
  void SetName(std::string_view name);

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
  void SetSubsystem(std::string_view subsystem);

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
   * Decorates this command with a timeout. If the specified timeout is
   * exceeded before the command finishes normally, the command will be
   * interrupted and un-scheduled.
   *
   * @param duration the timeout duration
   * @return the command with the timeout added
   */
  [[nodiscard]]
  CommandPtr WithTimeout(units::second_t duration) &&;

  /**
   * Decorates this command with an interrupt condition. If the specified
   * condition becomes true before the command finishes normally, the command
   * will be interrupted and un-scheduled.
   *
   * @param condition the interrupt condition
   * @return the command with the interrupt condition added
   */
  [[nodiscard]]
  CommandPtr Until(std::function<bool()> condition) &&;

  /**
   * Decorates this command with a run condition. If the specified condition
   * becomes false before the command finishes normally, the command will be
   * interrupted and un-scheduled.
   *
   * @param condition the run condition
   * @return the command with the run condition added
   */
  [[nodiscard]]
  CommandPtr OnlyWhile(std::function<bool()> condition) &&;

  /**
   * Decorates this command with a runnable to run before this command starts.
   *
   * @param toRun the Runnable to run
   * @param requirements the required subsystems
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr BeforeStarting(std::function<void()> toRun,
                            Requirements requirements = {}) &&;

  /**
   * Decorates this command with another command to run before this command
   * starts.
   *
   * @param before the command to run before this one
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr BeforeStarting(CommandPtr&& before) &&;

  /**
   * Decorates this command with a runnable to run after the command finishes.
   *
   * @param toRun the Runnable to run
   * @param requirements the required subsystems
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr AndThen(std::function<void()> toRun,
                     Requirements requirements = {}) &&;

  /**
   * Decorates this command with a set of commands to run after it in sequence.
   * Often more convenient/less-verbose than constructing a
   * SequentialCommandGroup explicitly.
   *
   * @param next the commands to run next
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr AndThen(CommandPtr&& next) &&;

  /**
   * Decorates this command to run repeatedly, restarting it when it ends, until
   * this command is interrupted. The decorated command can still be canceled.
   *
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr Repeatedly() &&;

  /**
   * Decorates this command to run "by proxy" by wrapping it in a ProxyCommand.
   * Use this for "forking off" from command compositions when the user does not
   * wish to extend the command's requirements to the entire command
   * composition. ProxyCommand has unique implications and semantics, see <a
   * href="https://docs.wpilib.org/en/stable/docs/software/commandbased/command-compositions.html#scheduling-other-commands">the
   * WPILib docs</a> for a full explanation.
   *
   * <p>This overload transfers command ownership to the returned CommandPtr.
   *
   * @return the decorated command
   * @see ProxyCommand
   */
  [[nodiscard]]
  CommandPtr AsProxy() &&;

  /**
   * Decorates this command to only run if this condition is not met. If the
   * command is already running and the condition changes to true, the command
   * will not stop running. The requirements of this command will be kept for
   * the new conditional command.
   *
   * @param condition the condition that will prevent the command from running
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr Unless(std::function<bool()> condition) &&;

  /**
   * Decorates this command to only run if this condition is met. If the command
   * is already running and the condition changes to false, the command will not
   * stop running. The requirements of this command will be kept for the new
   * conditional command.
   *
   * @param condition the condition that will allow the command to run
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr OnlyIf(std::function<bool()> condition) &&;

  /**
   * Creates a new command that runs this command and the deadline in parallel,
   * finishing (and interrupting this command) when the deadline finishes.
   *
   * @param deadline the deadline of the command group
   * @return the decorated command
   * @see DeadlineFor
   */
  CommandPtr WithDeadline(CommandPtr&& deadline) &&;

  /**
   * Decorates this command with a set of commands to run parallel to it, ending
   * when the calling command ends and interrupting all the others. Often more
   * convenient/less-verbose than constructing a new {@link
   * ParallelDeadlineGroup} explicitly.
   *
   * @param parallel the commands to run in parallel. Note the parallel commands
   * will be interupted when the deadline command ends
   * @return the decorated command
   * @see WithDeadline
   */
  [[nodiscard]]
  CommandPtr DeadlineFor(CommandPtr&& parallel) &&;

  /**
   * Decorates this command with a set of commands to run parallel to it, ending
   * when the last command ends. Often more convenient/less-verbose than
   * constructing a new {@link ParallelCommandGroup} explicitly.
   *
   * @param parallel the commands to run in parallel
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr AlongWith(CommandPtr&& parallel) &&;

  /**
   * Decorates this command with a set of commands to run parallel to it, ending
   * when the first command ends. Often more convenient/less-verbose than
   * constructing a new {@link ParallelRaceGroup} explicitly.
   *
   * @param parallel the commands to run in parallel
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr RaceWith(CommandPtr&& parallel) &&;

  /**
   * Decorates this command to run or stop when disabled.
   *
   * @param doesRunWhenDisabled true to run when disabled.
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr IgnoringDisable(bool doesRunWhenDisabled) &&;

  /**
   * Decorates this command to have a different interrupt behavior.
   *
   * @param interruptBehavior the desired interrupt behavior
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr WithInterruptBehavior(
      Command::InterruptionBehavior interruptBehavior) &&;

  /**
   * Decorates this command with a lambda to call on interrupt or end, following
   * the command's inherent Command::End(bool) method.
   *
   * @param end a lambda accepting a boolean parameter specifying whether the
   * command was interrupted.
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr FinallyDo(std::function<void(bool)> end) &&;

  /**
   * Decorates this command with a lambda to call on interrupt or end, following
   * the command's inherent Command::End(bool) method. The provided lambda will
   * run identically in both interrupt and end cases.
   *
   * @param end a lambda to run when the command ends, whether or not it was
   * interrupted.
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr FinallyDo(std::function<void()> end) &&;

  /**
   * Decorates this command with a lambda to call on interrupt, following the
   * command's inherent Command::End(bool) method.
   *
   * @param handler a lambda to run when the command is interrupted
   * @return the decorated command
   */
  [[nodiscard]]
  CommandPtr HandleInterrupt(std::function<void()> handler) &&;

  /**
   * Decorates this Command with a name.
   *
   * @param name name
   * @return the decorated Command
   */
  [[nodiscard]]
  CommandPtr WithName(std::string_view name) &&;

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
   * Whether or not the command is currently scheduled. Note that this does not
   * detect whether the command is in a composition, only whether it is directly
   * being run by the scheduler.
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
  bool IsComposed() const;

  /**
   * Sets whether the command is currently composed in a command composition.
   * Can be used to "reclaim" a command if a composition is no longer going to
   * use it.  NOT ADVISED!
   */
  void SetComposed(bool isComposed);

  /**
   * Get the stacktrace of where this command was composed, or an empty
   * optional. Intended for internal use.
   *
   * @return optional string representation of the composition site stack trace.
   */
  std::optional<std::string> GetPreviousCompositionSite() const;

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
   * Transfers ownership of this command to a unique pointer.  Used for
   * decorator methods.
   */
  virtual CommandPtr ToPtr() && = 0;

  void InitSendable(wpi::SendableBuilder& builder) override;

 protected:
  Command();

 private:
  /// Requirements set.
  wpi::SmallSet<Subsystem*, 4> m_requirements;

  std::optional<std::string> m_previousComposition;
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
